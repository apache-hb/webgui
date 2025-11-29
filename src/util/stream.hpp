#pragma once

#include <thread>
#include <atomic>
#include <optional>
#include <concurrentqueue.h>

namespace sm {
    enum class AsyncStreamState {
        eIdle,
        eStreaming,
    };

    template<typename T, typename E>
    class AsyncStream {
        struct Entry {
            uint32_t generation;
            T item;
        };

        std::atomic<AsyncStreamState> mState;
        uint32_t mGeneration;

        std::unique_ptr<std::jthread> mWorkerThread;
        moodycamel::ConcurrentQueue<Entry> mItemQueue;

        std::atomic<bool> mHasError{false};
        std::optional<E> mLastError;

        void reset() {
            mHasError.store(false);
            mLastError.reset();
            if (mWorkerThread) {
                mWorkerThread->request_stop();
            }
        }

    public:
        AsyncStream()
            : mState(AsyncStreamState::eIdle)
            , mGeneration(0)
        { }

        bool isWorking() const {
            return mState.load() == AsyncStreamState::eStreaming;
        }

        bool hasError() const {
            return mHasError.load();
        }

        E error() const {
            assert(hasError());
            return mLastError.value();
        }

        std::optional<T> pullItem() {
            Entry entry;
            if (mItemQueue.try_dequeue(entry)) {
                if (entry.generation == mGeneration) {
                    return entry.item;
                }
            }

            return std::nullopt;
        }

        void fail(E error) {
            mLastError = std::move(error);
            mHasError.store(true);
        }

        template<typename F>
        void run(F&& fn) {
            reset();

            mWorkerThread.reset(new std::jthread([this, fn = std::forward<F>(fn)](std::stop_token stop) {
                mState = AsyncStreamState::eStreaming;

                auto generation = ++mGeneration;

                auto add = [this, generation](T item) {
                    mItemQueue.enqueue({generation, std::move(item)});
                };

                auto err = [this](E error) {
                    fail(std::move(error));
                };

                fn(add, err, stop);

                mState = AsyncStreamState::eIdle;
            }));
        }
    };
}
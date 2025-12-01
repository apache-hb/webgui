#pragma once

#include <thread>
#include <vector>
#include <atomic>
#include <optional>
#include <span>
#include <concurrentqueue.h>

namespace sm {
    enum class AsyncDescribeState {
        eIdle,
        eFetching,
    };

    template<typename T, typename E>
    class AsyncDescribe {
        struct Entry {
            uint32_t generation;
            T item;
        };

        std::atomic<AsyncDescribeState> mState;
        uint32_t mGeneration;

        std::vector<T> mItems;
        std::unique_ptr<std::jthread> mWorkerThread;
        moodycamel::ConcurrentQueue<Entry> mItemQueue;

        std::atomic<bool> mHasError{false};
        std::optional<E> mLastError;

        void reset() {
            mItems.clear();
            mHasError.store(false);
            mLastError.reset();
            if (mWorkerThread) {
                mWorkerThread->request_stop();
            }
        }

        void pullItem() {
            Entry entry;
            if (mItemQueue.try_dequeue(entry)) {
                if (entry.generation == mGeneration) {
                    mItems.push_back(entry.item);
                }
            }
        }

    public:
        AsyncDescribe()
            : mState(AsyncDescribeState::eIdle)
            , mGeneration(0)
        { }

        bool isWorking() const {
            return mState.load() == AsyncDescribeState::eFetching;
        }

        bool hasError() const {
            return mHasError.load();
        }

        E error() const {
            assert(hasError());
            return mLastError.value();
        }

        std::span<const T> getItems() {
            pullItem();
            return mItems;
        }

        void fail(E error) {
            mLastError = std::move(error);
            mHasError.store(true);
        }

        void clear() {
            mHasError.store(false);
            mLastError.reset();
        }

        template<typename F>
        void run(F&& fn) {
            reset();

            mWorkerThread.reset(new std::jthread([this, fn = std::forward<F>(fn)](std::stop_token stop) {
                mState = AsyncDescribeState::eFetching;

                auto generation = ++mGeneration;

                auto add = [this, generation](T item) {
                    mItemQueue.enqueue({generation, std::move(item)});
                };

                auto err = [this](E error) {
                    fail(std::move(error));
                };

                fn(add, err, stop);

                mState = AsyncDescribeState::eIdle;
            }));
        }
    };

}
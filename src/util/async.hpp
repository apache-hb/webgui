#pragma once

#include <atomic>
#include <memory>
#include <optional>
#include <thread>
#include <cassert>

namespace sm {
    enum class AsyncActionState {
        eIdle,
        eRunning,
        eComplete,
    };

    template<typename T>
    class AsyncAction {
        std::atomic<AsyncActionState> mState;
        std::unique_ptr<std::jthread> mWorkerThread;

        std::optional<T> mResult;
    public:
        AsyncAction()
            : mState(AsyncActionState::eIdle)
        { }

        bool clear() {
            auto expected = AsyncActionState::eComplete;
            return mState.compare_exchange_strong(expected, AsyncActionState::eIdle);
        }

        void reset() {
            mResult.reset();
            if (mWorkerThread) {
                mWorkerThread->request_stop();
            }
        }

        void run(auto&& func) {
            if (isWorking()) {
                return;
            }

            reset();

            mState.store(AsyncActionState::eRunning);
            mResult.reset();

            mWorkerThread = std::make_unique<std::jthread>([this, func = std::move(func)]() {
                mResult = func();
                mState.store(AsyncActionState::eComplete);
            });
        }

        bool isWorking() const {
            return mState.load() == AsyncActionState::eRunning;
        }

        bool isComplete() const {
            return mState.load() == AsyncActionState::eComplete;
        }

        T getResult() const {
            assert(!isWorking());
            return mResult.value();
        }
    };
}

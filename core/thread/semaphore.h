#pragma once

#include <condition_variable>
#include <mutex>

#include "core/macros.h"

namespace ho {

#ifdef THREAD_ENABLED

    class Semaphore {
       public:
        explicit Semaphore(uint32_t count = 0) : count_(count) {}

        void Acquire() {
            std::unique_lock lock(mutex_);
            while (!count_) {
                cv_.wait(lock);
            }
            --count_;
        }

        bool TryAcquire() {
            std::unique_lock lock(mutex_);
            if (count_) {
                --count_;
                return true;
            }
            return false;
        }

        void Release() {
            std::unique_lock lock(mutex_);
            ++count_;
            cv_.notify_one();
        }

       private:
        uint32_t count_;
        std::condition_variable cv_;
        std::mutex mutex_;
    };

#else

    class Semaphore {
       public:
        explicit Semaphore(uint32_t = 0) {}

        void Acquire() {}
        bool TryAcquire() { return true; }
        void Release() {}
    };

#endif  // THREAD_ENABLED

}  // namespace ho

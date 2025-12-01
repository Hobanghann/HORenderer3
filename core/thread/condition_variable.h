#pragma once

#include <condition_variable>

#include "binary_recursive_mutex.h"
#include "mutex.h"

namespace ho {

#ifdef THREAD_ENABLED

    class ConditionVariable {
       public:
        template <typename BinaryMutexT>
        ALWAYS_INLINE void wait(const MutexLock<BinaryMutexT>& lock) const {
            cv_.wait(lock.lock_);
        }

        template <int Tag>
        ALWAYS_INLINE void wait(const MutexLock<BinaryRecursiveMutex<Tag>>& lock) const {
            cv_.wait(lock.mutex_.tls_lock_);
        }

        ALWAYS_INLINE void notify_one() const { cv_.notify_one(); }
        ALWAYS_INLINE void notify_all() const { cv_.notify_all(); }

       private:
        mutable std::condition_variable cv_;
    };

#else

    class ConditionVariable {
       public:
        template <typename BinaryMutexT>
        ALWAYS_INLINE void wait(const MutexLock<BinaryMutexT>&) const {
            // no-op in single-threaded mode
        }

        template <int Tag>
        ALWAYS_INLINE void wait(const MutexLock<BinaryRecursiveMutex<Tag>>&) const {
            // no-op in single-threaded mode
        }

        ALWAYS_INLINE void notify_one() const {}
        ALWAYS_INLINE void notify_all() const {}
    };

#endif  // THREAD_ENABLED

}  // namespace ho

#pragma once

#include <mutex>
#include <thread>

#include "core/macros.h"
#include "mutex.h"

// This class defines a recursive mutex that can be used with a condition
// variable. To achieve this, a lock and count are stored in TLS (thread-local
// storage) to avoid data races, while the underlying mutex is defined as a
// static member of the class so that the TLS lock can reference it. Therefore,
// instances of this mutex are distinguished by the class's template argument.

namespace ho {

#ifdef THREAD_ENABLED

    template <int Tag>
    class BinaryRecursiveMutex {
        friend class ConditionVariable;

       public:
        ALWAYS_INLINE void Lock() const {
            if (tls_lock_count_ == 0) {
                tls_lock_.lock();
            }
            ++tls_lock_count_;
        }

        ALWAYS_INLINE void Unlock() const {
            assert(tls_lock_count_ > 0);
            --tls_lock_count_;
            if (tls_lock_count_ == 0) {
                tls_lock_.unlock();
            }
        }

        ALWAYS_INLINE bool TryLock() const {
            if (tls_lock_count_ > 0) {
                ++tls_lock_count_;
                return true;
            }
            if (tls_lock_.try_lock()) {
                tls_lock_count_ = 1;
                return true;
            }
            return false;
        }

       private:
        static std::mutex mutex_;
        static thread_local std::unique_lock<std::mutex> tls_lock_;
        static thread_local uint32_t tls_lock_count_;
    };

    template <int Tag>
    std::mutex BinaryRecursiveMutex<Tag>::mutex_;

    template <int Tag>
    thread_local std::unique_lock<std::mutex> BinaryRecursiveMutex<Tag>::tls_lock_(BinaryRecursiveMutex<Tag>::mutex_,
                                                                                   std::defer_lock);

    template <int Tag>
    thread_local uint32_t BinaryRecursiveMutex<Tag>::tls_lock_count_ = 0;

    template <int Tag>
    class MutexLock<BinaryRecursiveMutex<Tag>> {
        friend class ConditionVariable;

       public:
        explicit MutexLock(const BinaryRecursiveMutex<Tag>& mutex) : mutex_(mutex) { mutex.Lock(); }

        ~MutexLock() { mutex.Unlock(); }

        ALWAYS_INLINE void Relock() const { mutex.Lock(); }

        ALWAYS_INLINE void Unlock() const { mutex.Unlock(); }

       private:
        const BinaryRecursiveMutex<Tag>& mutex_;
    };

#else

    template <int Tag>
    class BinaryRecursiveMutex {
       public:
        ALWAYS_INLINE void Lock() const {}
        ALWAYS_INLINE void Unlock() const {}
        ALWAYS_INLINE bool TryLock() const { return true; }
    };

    template <int Tag>
    class MutexLock<BinaryRecursiveMutex<Tag>> {
       public:
        explicit MutexLock(const BinaryRecursiveMutex<Tag>& mutex) {}

        ~MutexLock() {}

        ALWAYS_INLINE void Relock() const {}

        ALWAYS_INLINE void Unlock() const {}
    };

#endif  // THREAD_ENABLED

}  // namespace ho

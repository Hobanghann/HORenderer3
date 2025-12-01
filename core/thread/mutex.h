#pragma once

#include <mutex>

#include "core/macros.h"

namespace ho {

#ifdef THREAD_ENABLED

    template <typename MutexT>
    class MutexLock;

    template <typename STL_MutexT>
    class Mutex {
        friend class MutexLock<Mutex<STL_MutexT>>;

       public:
        using STL_MutexType = STL_MutexT;

        ALWAYS_INLINE void Lock() const { mutex_.lock(); }

        ALWAYS_INLINE void Unlock() const { mutex_.unlock(); }

        ALWAYS_INLINE bool TryLock() const { return mutex_.try_lock(); }

       private:
        mutable STL_MutexT mutex_;
    };

    using BinaryMutex = Mutex<std::mutex>;
    using RecursiveMutex = Mutex<std::recursive_mutex>;

    // Type Contracts
    static_assert(std::is_same_v<BinaryMutex, Mutex<std::mutex>>);
    static_assert(std::is_same_v<RecursiveMutex, Mutex<std::recursive_mutex>>);

    template <typename MutexT>
    class MutexLock {
        friend class ConditionVariable;

       public:
        ALWAYS_INLINE explicit MutexLock(const MutexT& mutex) : lock_(mutex.mutex_) {}

        ALWAYS_INLINE void Relock() const { lock_.lock(); }

        ALWAYS_INLINE void Unlock() const { lock_.unlock(); }

       private:
        mutable std::unique_lock<typename MutexT::STL_MutexType> lock_;
    };

#else

    template <typename STL_MutexT>
    class Mutex {
       public:
        ALWAYS_INLINE void Lock() const {}
        ALWAYS_INLINE void Unlock() const {}
        ALWAYS_INLINE bool TryLock() const { return true; }
    };

    using BinaryMutex = Mutex<std::mutex>;
    using RecursiveMutex = Mutex<std::recursive_mutex>;

    template <typename MutexT>
    class MutexLock {
        friend class ConditionVariable;

       public:
        ALWAYS_INLINE explicit MutexLock(const MutexT&) {}
        ALWAYS_INLINE void Relock() const {}
        ALWAYS_INLINE void Unlock() const {}
    };

#endif  // THREAD_ENABLED

}  // namespace ho

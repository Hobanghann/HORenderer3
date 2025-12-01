#pragma once

#include <atomic>

#include "core/macros.h"

namespace ho {
    template <typename T>
    class AtomicNumeric {
       public:
        ALWAYS_INLINE explicit AtomicNumeric(T value) { Set(value); }

        ALWAYS_INLINE void Set(T value) { value_.store(value, std::memory_order_release); }

        ALWAYS_INLINE T Get() const { return value_.load(std::memory_order_acquire); }

        ALWAYS_INLINE T Increment() { return value_.fetch_add(1, std::memory_order_acq_rel) + 1; }

        ALWAYS_INLINE T Decrement() { return value_.fetch_sub(1, std::memory_order_acq_rel) - 1; }

        ALWAYS_INLINE T Add(T value) { return value_.fetch_add(value, std::memory_order_acq_rel) + value; }

        ALWAYS_INLINE T Sub(T value) { return value_.fetch_sub(value, std::memory_order_acq_rel) - value; }

        ALWAYS_INLINE T ExchangeIfGreater(T new_val) {
            while (true) {
                T old = value_.load(std::memory_order_acquire);
                if (old >= new_val) {
                    return old;
                }

                if (value_.compare_exchange_weak(old, new_val, std::memory_order_acq_rel)) {
                    return new_val;
                }
            }
        }

        ALWAYS_INLINE bool TestEqual(T new_val) {
            T old = value_.load(std::memory_order_relaxed);
            return old == new_val;
        }

        ALWAYS_INLINE bool TestNotEqual(T new_val) {
            T old = value_.load(std::memory_order_relaxed);
            return old != new_val;
        }

        ALWAYS_INLINE bool TestLess(T new_val) {
            T old = value_.load(std::memory_order_relaxed);
            return old > new_val;
        }

        ALWAYS_INLINE bool TestGreater(T new_val) {
            T old = value_.load(std::memory_order_relaxed);
            return old < new_val;
        }

        ALWAYS_INLINE bool TestLessEqual(T new_val) {
            T old = value_.load(std::memory_order_relaxed);
            return old >= new_val;
        }

        ALWAYS_INLINE bool TestGreaterEqual(T new_val) {
            T old = value_.load(std::memory_order_relaxed);
            return old <= new_val;
        }

       private:
        std::atomic<T> value_;

        static_assert(std::atomic<T>::is_always_lock_free);
    };
}  // namespace ho
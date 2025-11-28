#pragma once

#include <atomic>

#include "core/macros.h"

namespace ho {
template <typename T>
class AtomicNumeric {
 public:
  _ALWAYS_INLINE_ explicit AtomicNumeric(T value) { Set(value); }

  _ALWAYS_INLINE_ void Set(T value) {
    value_.store(value, std::memory_order_release);
  }

  _ALWAYS_INLINE_ T Get() const {
    return value_.load(std::memory_order_acquire);
  }

  _ALWAYS_INLINE_ T Increment() {
    return value_.fetch_add(1, std::memory_order_acq_rel) + 1;
  }

  _ALWAYS_INLINE_ T Decrement() {
    return value_.fetch_sub(1, std::memory_order_acq_rel) - 1;
  }

  _ALWAYS_INLINE_ T Add(T value) {
    return value_.fetch_add(value, std::memory_order_acq_rel) + value;
  }

  _ALWAYS_INLINE_ T Sub(T value) {
    return value_.fetch_sub(value, std::memory_order_acq_rel) - value;
  }

  _ALWAYS_INLINE_ T ExchangeIfGreater(T new_val) {
    while (true) {
      T old = value_.load(std::memory_order_acquire);
      if (old >= new_val) {
        return old;
      }

      if (value_.compare_exchange_weak(old, new_val,
                                       std::memory_order_acq_rel)) {
        return new_val;
      }
    }
  }

  _ALWAYS_INLINE_ bool TestEqual(T new_val) {
    T old = value_.load(std::memory_order_relaxed);
    return old == new_val;
  }

  _ALWAYS_INLINE_ bool TestNotEqual(T new_val) {
    T old = value_.load(std::memory_order_relaxed);
    return old != new_val;
  }

  _ALWAYS_INLINE_ bool TestLess(T new_val) {
    T old = value_.load(std::memory_order_relaxed);
    return old > new_val;
  }

  _ALWAYS_INLINE_ bool TestGreater(T new_val) {
    T old = value_.load(std::memory_order_relaxed);
    return old < new_val;
  }

  _ALWAYS_INLINE_ bool TestLessEqual(T new_val) {
    T old = value_.load(std::memory_order_relaxed);
    return old >= new_val;
  }

  _ALWAYS_INLINE_ bool TestGreaterEqual(T new_val) {
    T old = value_.load(std::memory_order_relaxed);
    return old <= new_val;
  }

 private:
  std::atomic<T> value_;

  static_assert(std::atomic<T>::is_always_lock_free);
};
}  // namespace ho
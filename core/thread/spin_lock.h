#pragma once

#include <atomic>

#ifdef _MSC_VER
#include <intrin.h>
#endif

#include "core/macros.h"
#include "thread.h"

namespace ho {

    /*
     * This function is adapted from Godot Engine's spin_lock.h.
     *
     * The original work is licensed under the MIT License:
     *
     * Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.
     * Copyright (c) 2014-present Godot Engine contributors.
     *
     * Permission is hereby granted, free of charge, to any person obtaining a copy
     * of this software and associated documentation files (the "Software"), to deal
     * in the Software without restriction, including without limitation the rights
     * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
     * copies of the Software, and to permit persons to whom the Software is
     * furnished to do so, subject to the following conditions:
     *
     * The above copyright notice and this permission notice shall be included in
     * all copies or substantial portions of the Software.
     *
     * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
     * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
     * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
     * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
     * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
     * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
     * SOFTWARE.
     *
     * Local modifications: function name normalized to CPU_PAUSE and minor
     * formatting changes. No functional changes to the original logic.
     */

    ALWAYS_INLINE static void CPU_PAUSE() {
#if defined(_MSC_VER)
// ----- MSVC.
#if defined(_M_ARM) || defined(_M_ARM64)  // ARM.
        __yield();
#elif defined(_M_IX86) || defined(_M_X64)  // x86.
        _mm_pause();
#endif
#elif defined(__GNUC__) || defined(__clang__)
// ----- GCC/Clang.
#if defined(__i386__) || defined(__x86_64__)    // x86.
        __builtin_ia32_pause();
#elif defined(__arm__) || defined(__aarch64__)  // ARM.
        asm volatile("yield");
#elif defined(__powerpc__)                      // PowerPC.
        asm volatile("or 27,27,27");
#elif defined(__riscv)                          // RISC-V.
        asm volatile(".insn i 0x0F, 0, x0, x0, 0x010");
#endif
#endif
    }

#ifdef THREAD_ENABLED

    static_assert(std::atomic_bool::is_always_lock_free, "std::atomic<bool> must be lock-free for this SpinLock.");

    class SpinLock {
       public:
        ALWAYS_INLINE void Lock() const {
            while (true) {
                bool expected = false;
                if (is_locked.compare_exchange_weak(expected, true, std::memory_order_acquire,
                                                    std::memory_order_relaxed)) {
                    break;  // acquired
                }
                // brief backoff while contended
                do {
                    CPU_PAUSE();
                } while (is_locked.load(std::memory_order_relaxed));
            }
        }

        ALWAYS_INLINE void Unlock() const { is_locked.store(false, std::memory_order_release); }

       private:
        union {
            mutable std::atomic<bool> is_locked = false;
            char aligner[Thread::CACHE_LINE_BYTES];  // reduce false sharing
        };
    };

#else

    class SpinLock {
       public:
        ALWAYS_INLINE void Lock() const {}
        ALWAYS_INLINE void Unlock() const {}
    };

#endif  // THREADS_ENABLED

}  // namespace ho
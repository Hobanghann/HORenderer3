#pragma once

#include <cassert>

#ifndef ALWAYS_INLINE
#if defined(__GNUC__)
#define ALWAYS_INLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE inline
#endif
#endif

#ifndef INLINE
#define INLINE inline
#endif

#ifndef NO_INLINE
#if defined(__GNUC__)
#define NO_INLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define NO_INLINE __declspec(noinline)
#else
#define NO_INLINE
#endif
#endif

#define THREAD_ENABLED
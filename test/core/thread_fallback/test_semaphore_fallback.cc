#define THREAD_DISABLED
#include <gtest/gtest.h>

#include <atomic>
#include <thread>

#include "core/thread/semaphore.h"

using namespace ho;

TEST(SemaphoreFallbackTest, Acquire) {
    Semaphore sem(0);

    sem.Acquire();

    SUCCEED();
}

TEST(SemaphoreFallbackTest, TryAcquire) {
    Semaphore sem(0);

    EXPECT_TRUE(sem.TryAcquire());
    EXPECT_TRUE(sem.TryAcquire());
    EXPECT_TRUE(sem.TryAcquire());
}

TEST(SemaphoreFallbackTest, Release) {
    Semaphore sem(0);

    sem.Release();
    sem.Release();

    SUCCEED();
}

TEST(SemaphoreFallbackTest, Acquires) {
    Semaphore sem;

    for (int i = 0; i < 100000; i++) {
        sem.Acquire();  // should never block
    }

    SUCCEED();
}

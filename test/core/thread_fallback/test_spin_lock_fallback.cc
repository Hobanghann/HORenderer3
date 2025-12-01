#define THREAD_DISABLED
#include <gtest/gtest.h>

#include <atomic>
#include <thread>

#include "core/thread/spin_lock.h"

using namespace ho;

TEST(SpinLockFallbackTest, LockUnlock) {
    SpinLock lock;

    lock.Lock();
    lock.Unlock();

    SUCCEED();
}

TEST(SpinLockFallbackTest, MultipleLockUnlock) {
    SpinLock lock;

    for (int i = 0; i < 100; i++) {
        lock.Lock();
        lock.Unlock();
    }

    SUCCEED();
}

TEST(SpinLockFallbackTest, NoOpExecution) {
    SpinLock lock;
    std::atomic<int> val{0};

    auto work = [&]() {
        for (int i = 0; i < 10000; i++) {
            lock.Lock();
            val++;
            lock.Unlock();
        }
    };

    work();

    EXPECT_EQ(val.load(), 10000);
}

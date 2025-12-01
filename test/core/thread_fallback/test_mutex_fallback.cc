#define THREAD_DISABLED
#include <gtest/gtest.h>

#include <atomic>
#include <thread>

#include "core/thread/mutex.h"

using namespace ho;

////////////////////////////////////////////////////////////
// BinaryMutex Fallback Tests
////////////////////////////////////////////////////////////
TEST(BinaryMutexFallbackTest, TryLock) {
    BinaryMutex mutex;
    EXPECT_TRUE(mutex.TryLock());
    EXPECT_TRUE(mutex.TryLock());
    EXPECT_TRUE(mutex.TryLock());
}

TEST(BinaryMutexFallbackTest, LockUnlock) {
    BinaryMutex mutex;
    mutex.Lock();
    mutex.Lock();
    mutex.Unlock();
    mutex.Unlock();

    SUCCEED();
}

////////////////////////////////////////////////////////////
// RecursiveMutex Fallback Tests
////////////////////////////////////////////////////////////
TEST(RecursiveMutexFallbackTest, TryLock) {
    RecursiveMutex mutex;
    EXPECT_TRUE(mutex.TryLock());
    EXPECT_TRUE(mutex.TryLock());
    EXPECT_TRUE(mutex.TryLock());
}

TEST(RecursiveMutexFallbackTest, LockUnlock) {
    RecursiveMutex mutex;

    mutex.Lock();
    mutex.Lock();
    mutex.Unlock();
    mutex.Unlock();

    SUCCEED();
}

////////////////////////////////////////////////////////////
// MutexLock Fallback Tests
////////////////////////////////////////////////////////////
TEST(MutexLockFallbackTest, Constructor) {
    BinaryMutex mutex;
    MutexLock<BinaryMutex> lock(mutex);
    SUCCEED();
}

TEST(MutexLockFallbackTest, Unlock) {
    BinaryMutex mutex;
    MutexLock<BinaryMutex> lock(mutex);
    lock.Unlock();
    SUCCEED();
}

TEST(MutexLockFallbackTest, Relock) {
    BinaryMutex mutex;
    MutexLock<BinaryMutex> lock(mutex);
    lock.Relock();
    SUCCEED();
}

TEST(MutexLockFallbackTest, MultipleUnlockRelockg) {
    BinaryMutex mutex;
    MutexLock<BinaryMutex> lock(mutex);

    for (int i = 0; i < 10; i++) {
        lock.Unlock();
        lock.Relock();
    }

    SUCCEED();
}

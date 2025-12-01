#define THREAD_DISABLED
#include <gtest/gtest.h>

#include <atomic>
#include <thread>

#include "core/macros.h"
#include "core/thread/binary_recursive_mutex.h"

using namespace ho;

////////////////////////////////////////////////////////////
// BinaryRecursiveMutex<Tag> FALLBACK Tests
////////////////////////////////////////////////////////////
TEST(BinaryRecursiveMutexFallbackTest, TryLock) {
    BinaryRecursiveMutex<1> mutex;
    EXPECT_TRUE(mutex.TryLock());
}

TEST(BinaryRecursiveMutexFallbackTest, LockUnlock) {
    BinaryRecursiveMutex<1> mutex;

    mutex.Lock();
    mutex.Lock();
    mutex.Unlock();
    mutex.Unlock();

    SUCCEED();
}

TEST(BinaryRecursiveMutexFallbackTest, MultipleLocks) {
    BinaryRecursiveMutex<1> mutex;

    for (int i = 0; i < 10; i++) {
        mutex.Lock();
        EXPECT_TRUE(mutex.TryLock());
    }
}

////////////////////////////////////////////////////////////
// MutexLock<BinaryRecursiveMutex<Tag>> Fallback Tests
////////////////////////////////////////////////////////////
TEST(BinaryRecursiveMutexLockFallbackTest, Constructor) {
    BinaryRecursiveMutex<1> mutex;
    MutexLock<BinaryRecursiveMutex<1>> lock(mutex);
    SUCCEED();
}

TEST(BinaryRecursiveMutexLockFallbackTest, Unlock) {
    BinaryRecursiveMutex<1> mutex;
    MutexLock<BinaryRecursiveMutex<1>> lock(mutex);
    lock.Unlock();
    SUCCEED();
}

TEST(BinaryRecursiveMutexLockFallbackTest, Relock) {
    BinaryRecursiveMutex<1> mutex;
    MutexLock<BinaryRecursiveMutex<1>> lock(mutex);
    lock.Relock();
    SUCCEED();
}
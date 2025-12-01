#define THREAD_ENABLED
#include <gtest/gtest.h>

#include <atomic>
#include <thread>

#include "core/thread/binary_recursive_mutex.h"

using namespace ho;

////////////////////////////////////////////////////////////
// BinaryRecursiveMutex<Tag> Tests
////////////////////////////////////////////////////////////
TEST(BinaryRecursiveMutexTest, Constructor) {
    BinaryRecursiveMutex<0> mutex;

    EXPECT_TRUE(mutex.TryLock());
    mutex.Unlock();
}

TEST(BinaryRecursiveMutexTest, LockUnLock) {
    BinaryRecursiveMutex<0> mutex;

    mutex.Lock();
    mutex.Unlock();

    SUCCEED();
}

TEST(BinaryRecursiveMutexTest, TryLock) {
    BinaryRecursiveMutex<0> mutex;

    EXPECT_TRUE(mutex.TryLock());
    EXPECT_TRUE(mutex.TryLock());
    EXPECT_TRUE(mutex.TryLock());

    mutex.Unlock();
    mutex.Unlock();
    mutex.Unlock();
}

TEST(BinaryRecursiveMutexTest, RecursiveLock) {
    BinaryRecursiveMutex<0> mutex;

    mutex.Lock();
    mutex.Lock();
    mutex.Lock();

    mutex.Unlock();
    mutex.Unlock();
    mutex.Unlock();

    SUCCEED();
}

TEST(BinaryRecursiveMutexTest, RecursiveTryLock) {
    BinaryRecursiveMutex<0> mutex;

    EXPECT_TRUE(mutex.TryLock());
    EXPECT_TRUE(mutex.TryLock());
    EXPECT_TRUE(mutex.TryLock());

    mutex.Unlock();
    mutex.Unlock();
    mutex.Unlock();
}

TEST(BinaryRecursiveMutexTest, MutualExclusion) {
    BinaryRecursiveMutex<0> mutex;
    int counter = 0;

    auto work = [&]() {
        for (int i = 0; i < 5000; i++) {
            mutex.Lock();
            counter++;
            mutex.Unlock();
        }
    };

    std::thread t1(work);
    std::thread t2(work);

    t1.join();
    t2.join();

    EXPECT_EQ(counter, 10000);
}

TEST(BinaryRecursiveMutexTest, TagIndependenceTest) {
    BinaryRecursiveMutex<0> A;
    BinaryRecursiveMutex<1> B;

    std::atomic<int> countA{0};
    std::atomic<int> countB{0};

    auto workA = [&]() {
        for (int i = 0; i < 5000; i++) {
            A.Lock();
            countA++;
            A.Unlock();
        }
    };

    auto workB = [&]() {
        for (int i = 0; i < 5000; i++) {
            B.Lock();
            countB++;
            B.Unlock();
        }
    };

    std::thread t1(workA);
    std::thread t2(workB);
    std::thread t3(workA);
    std::thread t4(workB);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    EXPECT_EQ(countA.load(), 10000);
    EXPECT_EQ(countB.load(), 10000);
}

////////////////////////////////////////////////////////////
// MutexLock<BinaryRecursiveMutex<Tag>> Tests
////////////////////////////////////////////////////////////
TEST(BinaryRecursiveMutexLockTest, RAIIBehavior) {
    BinaryRecursiveMutex<0> mutex;

    {
        MutexLock<BinaryRecursiveMutex<0>> lock(mutex);
        {
            MutexLock<BinaryRecursiveMutex<0>> lock2(mutex);
        }
    }

    EXPECT_TRUE(mutex.TryLock());
    mutex.Unlock();
}

TEST(BinaryRecursiveMutexLockTest, DeepRecursiveLockUnlock) {
    BinaryRecursiveMutex<0> mutex;

    std::function<void(int)> func;
    func = [&](int d) {
        MutexLock<BinaryRecursiveMutex<0>> lock(mutex);
        if (d > 0) func(d - 1);
    };

    func(30);

    EXPECT_TRUE(true);
}

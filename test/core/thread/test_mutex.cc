#define THREAD_ENABLED
#include <gtest/gtest.h>

#include <atomic>
#include <thread>

#include "core/thread/mutex.h"

using namespace ho;

////////////////////////////////////////////////////////////
// BinaryMutex Test
////////////////////////////////////////////////////////////
TEST(BinaryMutexTest, Constructor) {
    BinaryMutex mutex;

    EXPECT_TRUE(mutex.TryLock());
    mutex.Unlock();
}

TEST(BinaryMutexTest, LockUnlock) {
    BinaryMutex mutex;

    // Should lock and unlock without issues.
    mutex.Lock();
    mutex.Unlock();

    SUCCEED();
}

TEST(BinaryMutexTest, TryLock) {
    BinaryMutex mutex;
    mutex.Lock();
    EXPECT_FALSE(mutex.TryLock());
    mutex.Unlock();

    EXPECT_TRUE(mutex.TryLock());
    mutex.Unlock();
}

TEST(BinaryMutexTest, TryLockFail) {
    BinaryMutex mutex;

    mutex.Lock();

    std::atomic<bool> other_failed{false};

    std::thread t([&]() {
        if (!mutex.TryLock()) {
            other_failed = true;
        }
    });

    t.join();

    mutex.Unlock();

    EXPECT_TRUE(other_failed);
}

TEST(BinaryMutexTest, MutualExclusion) {
    BinaryMutex mutex;
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

    EXPECT_EQ(counter, 10000);  // must be exact
}

////////////////////////////////////////////////////////////
// RecursiveMutex Test
////////////////////////////////////////////////////////////
TEST(RecursiveMutexTest, Constructor) {
    RecursiveMutex mutex;
    EXPECT_TRUE(mutex.TryLock());
    mutex.Unlock();
}

TEST(RecursiveMutexTest, LockUnlock) {
    RecursiveMutex mutex;

    // Should lock and unlock without issues.
    mutex.Lock();
    mutex.Unlock();

    SUCCEED();
}

TEST(RecursiveMutexTest, TryLock) {
    RecursiveMutex mutex;

    EXPECT_TRUE(mutex.TryLock());

    // Recursive TryLock should succeed on same thread
    EXPECT_TRUE(mutex.TryLock());
    EXPECT_TRUE(mutex.TryLock());

    mutex.Unlock();
    mutex.Unlock();
    mutex.Unlock();
}

TEST(RecursiveMutexTest, TryLockFail) {
    RecursiveMutex mutex;

    mutex.Lock();

    std::atomic<bool> other_failed{false};

    std::thread t([&]() {
        if (!mutex.TryLock()) {
            other_failed = true;
        }
    });

    t.join();

    mutex.Unlock();

    EXPECT_TRUE(other_failed);
}

TEST(RecursiveMutexTest, MutualExclusion) {
    RecursiveMutex mutex;
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

TEST(RecursiveMutexTest, RecursiveLock) {
    RecursiveMutex mutex;

    mutex.Lock();
    mutex.Lock();
    mutex.Lock();

    mutex.Unlock();
    mutex.Unlock();
    mutex.Unlock();

    SUCCEED();
}

TEST(RecursiveMutexTest, RecursiveDepthWorks) {
    RecursiveMutex mutex;

    std::function<void(int& counter, int depth)> recurse;
    recurse = [&](int& counter, int depth) {
        mutex.Lock();
        counter++;

        if (depth > 0) {
            recurse(counter, depth - 1);
        }

        mutex.Unlock();
    };

    int value = 0;
    recurse(value, 5);

    EXPECT_EQ(value, 6);
}

////////////////////////////////////////////////////////////
// MutexLock Test
////////////////////////////////////////////////////////////
TEST(MutexLockTest, RAIIBehavior) {
    BinaryMutex mutex;

    {
        MutexLock lock(mutex);
    }

    EXPECT_TRUE(mutex.TryLock());
}

TEST(MutexLockTest, LockWithMultiThread) {
    BinaryMutex mutex;
    std::atomic<int> counter{0};

    auto worker = [&]() {
        for (int i = 0; i < 5000; i++) {
            MutexLock lock(mutex);
            counter.fetch_add(1, std::memory_order_relaxed);
        }
    };

    std::thread t1(worker);
    std::thread t2(worker);
    std::thread t3(worker);

    t1.join();
    t2.join();
    t3.join();

    EXPECT_EQ(counter.load(), 15000);
}

TEST(MutexLockTest, RelockAndUnlock) {
    BinaryMutex mutex;

    MutexLock lock(mutex);
    lock.Unlock();

    lock.Relock();
    lock.Unlock();

    SUCCEED();
}

TEST(MutexLockTest, MultipleRelockAndUnlock) {
    BinaryMutex mutex;

    MutexLock lock(mutex);

    for (int i = 0; i < 10; i++) {
        lock.Unlock();
        lock.Relock();
    }

    lock.Unlock();
    EXPECT_TRUE(mutex.TryLock());
    mutex.Unlock();
}

TEST(MutexLockTest, DeepRecursiveLockUnlock) {
    RecursiveMutex mutex;

    std::function<void(int)> recurse = [&](int depth) {
        MutexLock lock(mutex);
        if (depth > 0) recurse(depth - 1);
    };

    recurse(50);

    SUCCEED();
}

TEST(MutexLockTest, RecursiveLockCountWorks) {
    RecursiveMutex mutex;
    int value = 0;

    std::function<void(int)> func = [&](int depth) {
        MutexLock lock(mutex);
        value += 1;

        if (depth > 0) {
            func(depth - 1);
        }
    };

    func(5);  // lock 6 times recursively

    EXPECT_EQ(value, 6);
}

TEST(MutexLockTest, DestructorAfterRelock) {
    BinaryMutex mutex;

    {
        MutexLock lock(mutex);
        lock.Unlock();
        lock.Relock();
        // destructor should unlock
    }

    EXPECT_TRUE(mutex.TryLock());
    mutex.Unlock();
}

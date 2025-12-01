#define THREAD_ENABLED
#include <gtest/gtest.h>

#include <atomic>
#include <thread>

#include "core/thread/semaphore.h"

using namespace ho;

TEST(SemaphoreTest, Constructor) {
    Semaphore sem(2);

    EXPECT_TRUE(sem.TryAcquire());
    EXPECT_TRUE(sem.TryAcquire());
    EXPECT_FALSE(sem.TryAcquire());  // no count left
}

TEST(SemaphoreTest, AcquireBlock) {
    Semaphore sem(0);
    std::atomic<bool> acquired{false};

    std::thread worker([&]() {
        sem.Acquire();  // should block
        acquired = true;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    EXPECT_FALSE(acquired.load());  // still blocked

    sem.Release();  // allow the waiting thread to continue

    worker.join();
    EXPECT_TRUE(acquired.load());
}

TEST(SemaphoreTest, TryAcquire) {
    Semaphore sem(1);

    EXPECT_TRUE(sem.TryAcquire());   // should succeed
    EXPECT_FALSE(sem.TryAcquire());  // should fail

    sem.Release();
    EXPECT_TRUE(sem.TryAcquire());
}

TEST(SemaphoreTest, AcquireWithMultipleThreads) {
    const int PERMITS = 3;
    Semaphore sem(PERMITS);

    std::atomic<int> acquired_count{0};

    std::vector<std::thread> threads;
    for (int i = 0; i < PERMITS; i++) {
        threads.emplace_back([&]() {
            sem.Acquire();
            acquired_count++;
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(acquired_count.load(), PERMITS);
    EXPECT_FALSE(sem.TryAcquire());  // no more permits
}

TEST(SemaphoreTest, ReleaseWakesOneThread) {
    Semaphore sem(0);
    std::atomic<int> awakened{0};

    std::thread t1([&]() {
        sem.Acquire();
        awakened++;
    });
    std::thread t2([&]() {
        sem.Acquire();
        awakened++;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    EXPECT_EQ(awakened.load(), 0);

    // release one
    sem.Release();

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    EXPECT_EQ(awakened.load(), 1);

    // release the second
    sem.Release();

    t1.join();
    t2.join();

    EXPECT_EQ(awakened.load(), 2);
}

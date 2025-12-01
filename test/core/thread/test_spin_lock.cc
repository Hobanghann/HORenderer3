#define THREAD_ENABLED
#include <gtest/gtest.h>

#include <atomic>
#include <thread>

#include "core/thread/spin_lock.h"

using namespace ho;

TEST(SpinLockTest, BasicLockUnlock) {
    SpinLock lock;
    lock.Lock();
    lock.Unlock();
    SUCCEED();
}

TEST(SpinLockTest, SimpleContention) {
    SpinLock lock;
    std::atomic<int> counter{0};

    auto work = [&]() {
        for (int i = 0; i < 20000; i++) {
            lock.Lock();
            counter++;
            lock.Unlock();
        }
    };

    std::thread t1(work);
    std::thread t2(work);
    std::thread t3(work);

    t1.join();
    t2.join();
    t3.join();

    EXPECT_EQ(counter.load(), 60000);
}

TEST(SpinLockTest, MultiThreadContention) {
    SpinLock lock;
    std::atomic<int> counter{0};

    const int THREAD_COUNT = 8;
    const int INCREMENTS = 10000;

    std::vector<std::thread> threads;
    threads.reserve(THREAD_COUNT);

    for (int i = 0; i < THREAD_COUNT; i++) {
        threads.emplace_back([&]() {
            for (int k = 0; k < INCREMENTS; k++) {
                lock.Lock();
                counter++;
                lock.Unlock();
            }
        });
    }

    for (auto& t : threads) t.join();

    EXPECT_EQ(counter.load(), THREAD_COUNT * INCREMENTS);
}

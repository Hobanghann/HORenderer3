#define THREAD_ENABLED
#include <gtest/gtest.h>

#include <atomic>
#include <thread>

#include "core/thread/condition_variable.h"

using namespace ho;

TEST(ConditionVariableTest, WaitNotifyWithBinaryMutex) {
    BinaryMutex mutex;
    ConditionVariable cv;
    bool ready = false;

    std::thread worker([&]() {
        MutexLock lock(mutex);
        while (!ready) {
            cv.wait(lock);
        }
        // awakened correctly
        ready = false;
    });

    // let worker start & block
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    {
        MutexLock lock(mutex);
        ready = true;
        cv.notify_one();
    }

    worker.join();
    SUCCEED();
}

TEST(ConditionVariableTest, WaitNotifyWithBinaryRecursiveMutex) {
    BinaryRecursiveMutex<0> mutex;
    ConditionVariable cv;
    bool ready = false;

    std::thread worker([&]() {
        MutexLock<BinaryRecursiveMutex<0>> lock(mutex);

        while (!ready) cv.wait(lock);

        ready = false;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    {
        MutexLock<BinaryRecursiveMutex<0>> lock(mutex);
        ready = true;
        cv.notify_one();
    }

    worker.join();
    SUCCEED();
}

TEST(ConditionVariableTest, NotifyAllWithMultipleThreads) {
    BinaryMutex mutex;
    ConditionVariable cv;
    bool go = false;
    const int THREAD_COUNT = 5;

    std::atomic<int> awakened{0};

    std::vector<std::thread> threads;
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads.emplace_back([&]() {
            MutexLock lock(mutex);
            while (!go) {
                cv.wait(lock);
            }
            awakened++;
        });
    }

    // give all threads time to reach wait()
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    {
        MutexLock lock(mutex);
        go = true;
        cv.notify_all();
    }

    for (auto& t : threads) t.join();

    EXPECT_EQ(awakened.load(), THREAD_COUNT);
}

TEST(ConditionVariableTest, SpuriousWakeUpSafe) {
    BinaryMutex mutex;
    ConditionVariable cv;

    bool ready = false;
    int wake_count = 0;

    std::thread worker([&]() {
        MutexLock lock(mutex);
        while (!ready) {
            cv.wait(lock);
            wake_count++;
        }
    });

    // force one notify without setting ready
    {
        MutexLock lock(mutex);
        cv.notify_one();  // normal usage may cause wakeups
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // now set ready true
    {
        MutexLock lock(mutex);
        ready = true;
        cv.notify_one();
    }

    worker.join();

    EXPECT_GE(wake_count, 1);  // must have woken at least once
}

TEST(ConditionVariableTest, NotifyOneWakesOneThread) {
    BinaryMutex mutex;
    ConditionVariable cv;

    bool ready = false;
    std::atomic<int> awakened{0};

    MutexLock lock(mutex);

    std::thread t([&]() {
        MutexLock lock(mutex);
        while (!ready) cv.wait(lock);
        awakened++;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    ready = true;
    cv.notify_one();

    lock.Unlock();

    t.join();

    EXPECT_EQ(awakened.load(), 1);
}

TEST(ConditionVariableTest, NotifyAllWithoutWaitingThread) {
    ConditionVariable cv;

    // no threads are waiting
    cv.notify_all();  // should not crash or block

    SUCCEED();
}

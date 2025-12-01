#define THREAD_ENABLED
#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>

#include "core/thread/thread.h"

using namespace ho;

TEST(ThreadTest, EntryFunctionExecution) {
    std::atomic<int> value{0};

    auto entry = [](void* userdata) {
        auto* counter = reinterpret_cast<std::atomic<int>*>(userdata);
        counter->fetch_add(1, std::memory_order_relaxed);
    };

    Thread t(entry, &value);
    t.Join();

    EXPECT_EQ(value.load(), 1);
}

TEST(ThreadTest, MultipleThreadsExecution) {
    std::atomic<int> counter{0};

    auto entry = [](void* userdata) {
        auto* cnt = reinterpret_cast<std::atomic<int>*>(userdata);
        for (int i = 0; i < 5000; i++) {
            cnt->fetch_add(1, std::memory_order_relaxed);
        }
    };

    Thread t1(entry, &counter);
    Thread t2(entry, &counter);
    Thread t3(entry, &counter);

    t1.Join();
    t2.Join();
    t3.Join();

    EXPECT_EQ(counter.load(), 15000);
}

TEST(ThreadTest, PassesUserdataCorrectly) {
    struct Data {
        int x;
        int y;
    } test_data{2, 3};

    auto entry = [](void* userdata) {
        Data* d = reinterpret_cast<Data*>(userdata);
        d->x += d->y;  // 5
    };

    Thread t(entry, &test_data);
    t.Join();

    EXPECT_EQ(test_data.x, 5);
}

TEST(ThreadTest, Destructor) {
    std::atomic<int> value{0};

    {
        auto entry = [](void* userdata) {
            auto* v = reinterpret_cast<std::atomic<int>*>(userdata);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            v->store(42, std::memory_order_relaxed);
        };

        Thread t(entry, &value);
        // destructor should implicitly join when going out of scope
    }

    EXPECT_EQ(value.load(), 42);
}

TEST(ThreadTest, Yield) {
    for (int i = 0; i < 10; i++) {
        Thread::Yield();
    }
    SUCCEED();
}

TEST(ThreadTest, Sleep) {
    auto start = std::chrono::high_resolution_clock::now();
    Thread::Sleep(20);
    auto end = std::chrono::high_resolution_clock::now();

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    EXPECT_GE(elapsed, 15);
    EXPECT_LT(elapsed, 200);
}

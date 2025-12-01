#define THREAD_DISABLED
#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>

#include "core/thread/thread.h"

using namespace ho;

TEST(ThreadFallbackTest, EntryFunctionExecution) {
    std::atomic<int> value{0};

    auto entry = [](void* userdata) { reinterpret_cast<std::atomic<int>*>(userdata)->store(123); };

    Thread t(entry, &value);
    EXPECT_EQ(value.load(), 123);
}

TEST(ThreadFallbackTest, Join) {
    std::atomic<int> dummy{0};
    Thread t([](void*) {}, &dummy);

    auto start = std::chrono::high_resolution_clock::now();
    t.Join();
    auto end = std::chrono::high_resolution_clock::now();

    EXPECT_LT(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(), 5);
}

TEST(ThreadFallbackTest, Yield) {
    auto start = std::chrono::high_resolution_clock::now();
    Thread::Yield();
    auto end = std::chrono::high_resolution_clock::now();

    EXPECT_LT(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(), 5);
}

TEST(ThreadFallbackTest, Sleep) {
    auto start = std::chrono::high_resolution_clock::now();
    Thread::Sleep(50);
    auto end = std::chrono::high_resolution_clock::now();

    EXPECT_LT(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(), 5);
}

TEST(ThreadFallbackTest, CacheLineSizeIsPointerSize) { EXPECT_EQ(Thread::CACHE_LINE_BYTES, sizeof(void*)); }

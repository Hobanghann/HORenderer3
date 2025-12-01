#define THREAD_DISABLED
#include <gtest/gtest.h>

#include <atomic>
#include <thread>

#include "core/thread/condition_variable.h"

using namespace ho;

TEST(ConditionVariableFallbackTest, Wait) {
    BinaryMutex mutex;
    ConditionVariable cv;

    std::atomic<int> hit{0};

    auto entry = [&](void*) {
        MutexLock lock(mutex);
        cv.wait(lock);
        hit++;
    };

    entry(nullptr);

    EXPECT_EQ(hit.load(), 1);
}

TEST(ConditionVariableFallbackTest, Notify) {
    ConditionVariable cv;
    cv.notify_one();
    cv.notify_all();
    SUCCEED();
}

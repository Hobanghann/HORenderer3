#define THREAD_ENABLED
#include <gtest/gtest.h>

#include <atomic>
#include <thread>
#include <vector>

#include "core/thread/job_system.h"

using namespace ho;

static void AddInt(void* data, uint32_t size) {
    EXPECT_EQ(size, sizeof(int));
    int* p = reinterpret_cast<int*>(data);
    (*p)++;
}

TEST(JobSystemTest, KickSingleJob) {
    JobSystem js(4);

    int value = 0;
    JobDeclaration job{AddInt, &value, sizeof(int), nullptr};

    std::cerr << "before js.KickJob(job)" << std::endl;
    js.KickJob(job);
    std::cerr << "after js.KickJob(job)" << std::endl;
    js.WaitForIdle();
    std::cerr << "after js.WaitForIdle()" << std::endl;

    EXPECT_EQ(value, 1);
}

TEST(JobSystemTest, KickMultipleJobs) {
    JobSystem js(4);

    const int JOB_COUNT = 32;
    std::vector<int> values(JOB_COUNT, 0);

    std::vector<JobDeclaration> jobs;
    jobs.reserve(JOB_COUNT);

    for (int i = 0; i < JOB_COUNT; i++) {
        jobs.push_back({AddInt, &values[i], sizeof(int), nullptr});
    }

    js.KickJobs(jobs);
    js.WaitForIdle();

    for (auto v : values) EXPECT_EQ(v, 1);
}

TEST(JobSystemTest, KickJobAndWait) {
    JobSystem js(4);

    int v = 0;
    JobDeclaration job{AddInt, &v, sizeof(int), nullptr};

    js.KickJobAndWait(job);

    EXPECT_EQ(v, 1);
}

TEST(JobSystemTest, KickJobsAndWait) {
    JobSystem js(4);

    const int JOB_COUNT = 50;
    std::vector<int> values(JOB_COUNT, 0);

    std::vector<JobDeclaration> jobs;
    jobs.reserve(JOB_COUNT);

    for (int i = 0; i < JOB_COUNT; i++) {
        jobs.push_back({AddInt, &values[i], sizeof(int), nullptr});
    }

    js.KickJobsAndWait(jobs);

    for (auto v : values) EXPECT_EQ(v, 1);
}

TEST(JobSystemTest, CounterBasedSynchronization) {
    JobSystem js(4);

    const int JOB_COUNT = 16;
    std::vector<int> values(JOB_COUNT, 0);
    auto counter = std::make_shared<AtomicNumeric<uint32_t>>(JOB_COUNT);

    for (int i = 0; i < JOB_COUNT; i++) {
        JobDeclaration j{AddInt, &values[i], sizeof(int), counter};
        js.KickJob(j);
    }

    js.WaitForCounter(counter);

    for (auto v : values) EXPECT_EQ(v, 1);
}

TEST(JobSystemTest, StressTest) {
    JobSystem js(8);

    const int JOB_COUNT = 200;
    std::vector<int> values(JOB_COUNT, 0);

    std::vector<JobDeclaration> jobs;
    jobs.reserve(JOB_COUNT);

    for (int i = 0; i < JOB_COUNT; i++) {
        jobs.push_back({AddInt, &values[i], sizeof(int), nullptr});
    }

    js.KickJobs(jobs);

    js.WaitForIdle();
    for (auto v : values) EXPECT_EQ(v, 1);
}

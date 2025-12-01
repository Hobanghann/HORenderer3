#define THREAD_DISABLED
#include <gtest/gtest.h>

#include <vector>

#include "core/thread/job_system.h"

using namespace ho;
static void AddInt(void* data, uint32_t size) {
    EXPECT_EQ(size, sizeof(int));
    int* p = reinterpret_cast<int*>(data);
    (*p)++;
}

TEST(JobSystemFallbackTest, KickSingleJob) {
    JobSystem js(1);

    int v = 0;
    JobDeclaration job{AddInt, &v, sizeof(int), nullptr};

    js.KickJob(job);

    EXPECT_EQ(v, 1);
}

TEST(JobSystemFallbackTest, KickJobs) {
    JobSystem js(1);

    const int JOB_COUNT = 16;
    std::vector<int> values(JOB_COUNT, 0);

    std::vector<JobDeclaration> jobs;
    jobs.reserve(JOB_COUNT);

    for (size_t i = 0; i < JOB_COUNT; i++) {
        jobs.push_back({AddInt, &values[i], sizeof(int), nullptr});
    }

    js.KickJobs(jobs);

    for (auto v : values) EXPECT_EQ(v, 1);
}

TEST(JobSystemFallbackTest, KickJobAndWait) {
    JobSystem js(1);

    int v = 0;
    JobDeclaration job{AddInt, &v, sizeof(int), nullptr};

    js.KickJobAndWait(job);
    EXPECT_EQ(v, 1);
}

TEST(JobSystemFallbackTest, KickJobsAndWait) {
    JobSystem js(1);

    const int JOB_COUNT = 25;
    std::vector<int> values(JOB_COUNT, 0);

    std::vector<JobDeclaration> jobs;
    jobs.reserve(JOB_COUNT);

    for (size_t i = 0; i < JOB_COUNT; i++) {
        jobs.push_back({AddInt, &values[i], sizeof(int), nullptr});
    }

    js.KickJobsAndWait(jobs);

    for (auto v : values) EXPECT_EQ(v, 1);
}

TEST(JobSystemFallbackTest, CounterHandledCorrectly) {
    JobSystem js(1);

    const int JOB_COUNT = 10;
    std::vector<int> values(JOB_COUNT, 0);

    auto counter = std::make_shared<AtomicNumeric<uint32_t>>(JOB_COUNT);

    for (size_t i = 0; i < JOB_COUNT; i++) {
        JobDeclaration j{AddInt, &values[i], sizeof(int), counter};
        js.KickJob(j);
    }

    EXPECT_EQ(counter->Get(), 0u);
}

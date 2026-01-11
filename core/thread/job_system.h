#pragma once

#include <memory>
#include <queue>
#include <vector>

#include "condition_variable.h"
#include "core/templates/atomic_numeric.h"
#include "mutex.h"
#include "spin_lock.h"
#include "thread.h"

namespace ho {

    // A single job to be executed by the JobSystem
    struct JobDeclaration {
        using Entry = void (*)(void* input, uint32_t size);
        Entry entry = nullptr;                                  // job function pointer
        void* input_data = nullptr;                             // input payload
        uint32_t input_size = 0;                                // size of input payload
        std::shared_ptr<AtomicNumeric<std::uint32_t>> counter;  // completion counter
    };

// JobSystem: Manages workers and job scheduling
#ifdef THREAD_ENABLED
    class JobSystem {
       public:
        // Nested Worker class
        class Worker : public Thread {
           public:
            explicit Worker(JobSystem* owner) : Thread(&Worker::WorkerLoop, owner) {}

           private:
            // worker's entry function
            static void WorkerLoop(void* owner) {
                JobSystem* job_sys = static_cast<JobSystem*>(owner);

                while (true) {
                    JobDeclaration job;
                    {
                        MutexLock lock(job_sys->mutex_);
                        while (job_sys->job_queue_.empty() && job_sys->is_running_) {
                            job_sys->cv_.wait(lock);  // wait until job is kicked in queue
                        }
                        if (!job_sys->is_running_) {
                            return;  // thread exit
                        }
                        job = job_sys->job_queue_.front();
                        job_sys->job_queue_.pop();
                    }

                    job.entry(job.input_data, job.input_size);

                    if (job.counter != nullptr) {
                        // if job was waited by job system
                        // decrement count and if count is zero, wake up main thread
                        if (job.counter->Decrement() == 0) {
                            MutexLock lock(job_sys->mutex_);
                            job_sys->cv_.notify_all();
                        }
                    }

                    if (job_sys->job_count_.Decrement() == 0) {
                        MutexLock lock(job_sys->mutex_);
                        job_sys->cv_.notify_all();  // for wake up main thread
                    }
                }
            }
        };

       public:
        explicit JobSystem(uint32_t worker_count) : is_running_(true) {
            assert(worker_count != 0);
            worker_pool_.reserve(worker_count);
            for (uint32_t i = 0; i < worker_count; i++) {
                worker_pool_.emplace_back(std::make_unique<Worker>(this));
            }
        }

        ~JobSystem() {
            {
                MutexLock lock(mutex_);
                is_running_ = false;
                cv_.notify_all();
            }

            for (auto& w : worker_pool_) {
                w->Join();
            }
        }

        // Job submission
        void KickJob(const JobDeclaration& job) {
            MutexLock lock(mutex_);
            job_count_.Increment();
            job_queue_.push(job);
            cv_.notify_one();
        }

        void KickJobs(const std::vector<JobDeclaration>& jobs) {
            MutexLock lock(mutex_);
            job_count_.Add((uint32_t)jobs.size());
            for (const JobDeclaration& job : jobs) {
                job_queue_.push(job);
            }
            cv_.notify_all();
        }

        void KickJobAndWait(const JobDeclaration& job) {
            auto counter = std::make_shared<AtomicNumeric<uint32_t>>(1);
            JobDeclaration j = job;
            j.counter = counter;
            KickJob(j);
            WaitForCounter(counter);
        }

        void KickJobsAndWait(const std::vector<JobDeclaration>& jobs) {
            if (jobs.empty()) return;
            auto counter = std::make_shared<AtomicNumeric<uint32_t>>(static_cast<uint32_t>(jobs.size()));
            std::vector<JobDeclaration> with_counter;
            with_counter.reserve(jobs.size());

            for (auto job : jobs) {
                job.counter = counter;
                with_counter.push_back(job);
            }
            KickJobs(with_counter);
            WaitForCounter(counter);
        }

        // Job synchronization
        void WaitForCounter(std::shared_ptr<AtomicNumeric<std::uint32_t>> counter) {
            int spin_count = 100;
            while (counter->Get() > 0 && spin_count) {
                if (counter->Get() == 0) return;
                CPU_PAUSE();
            }
            if (counter->Get() == 0) return;

            MutexLock lock(mutex_);
            while (counter->Get() > 0) {
                cv_.wait(lock);
            }
        }

        void WaitForIdle() {
            for (int i = 0; i < 100; ++i) {
                if (job_count_.Get() == 0) return;
                CPU_PAUSE();
            }

            MutexLock lock(mutex_);

            while (job_count_.Get() > 0) {
                cv_.wait(lock);
            }
        }

       private:
        BinaryMutex mutex_;
        ConditionVariable cv_;
        std::queue<JobDeclaration> job_queue_;
        std::vector<std::unique_ptr<Worker>> worker_pool_;
        AtomicNumeric<uint32_t> job_count_{0};
        bool is_running_;
    };

#else

    // Single-threaded fallback version (no threads)
    class JobSystem {
       public:
        explicit JobSystem(uint32_t) {}

        void KickJob(const JobDeclaration& job) {
            job.entry(job.input_data, job.input_size);
            if (job.counter != nullptr) {
                job.counter->Decrement();
            }
        }

        void KickJobs(const std::vector<JobDeclaration>& jobs) {
            for (auto& job : jobs) {
                KickJob(job);
            }
        }

        void WaitForCounter(std::shared_ptr<AtomicNumeric<std::uint32_t>> counter) {
            (void)counter;  // no-op, all jobs run inline
        }

        void KickJobAndWait(const JobDeclaration& job) {
            auto counter = std::make_shared<AtomicNumeric<uint32_t>>(1);
            JobDeclaration j = job;
            j.counter = counter;
            KickJob(j);
        }

        void KickJobsAndWait(const std::vector<JobDeclaration>& jobs) {
            if (jobs.empty()) return;
            auto counter = std::make_shared<AtomicNumeric<uint32_t>>(static_cast<uint32_t>(jobs.size()));
            std::vector<JobDeclaration> with_counter;
            with_counter.reserve(jobs.size());
            for (auto job : jobs) {
                job.counter = counter;
                with_counter.push_back(job);
            }
            KickJobs(with_counter);
        }
    };

#endif  // THREAD_ENABLED

}  // namespace ho

#include "thread.h"

#ifdef THREAD_ENABLED

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// clang-format off
#include <windows.h>
#include <processthreadsapi.h>
// clang-format on

#elif defined(__linux__)
#include <pthread.h>
#endif

namespace ho {

    Thread::~Thread() {
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    void Thread::Join() {
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    void Thread::SetName(const std::string& name) {
#ifdef _WIN32
        std::wstring wname(name.begin(), name.end());
        SetThreadDescription(static_cast<HANDLE>(thread_.native_handle()), wname.c_str());
#elif defined(__linux__)
        pthread_setname_np(thread_.native_handle(), name.c_str());
#endif
    }

    void Thread::SetCurrentName(const std::string& name) {
#ifdef _WIN32
        std::wstring wname(name.begin(), name.end());
        SetThreadDescription(GetCurrentThread(), wname.c_str());
#elif defined(__linux__)
        pthread_setname_np(pthread_self(), name.c_str());
#endif
    }

}  // namespace ho

#endif  // THREAD_ENABLED
#pragma once

#include <chrono>
#include <string>
#include <thread>

#include "core/macros.h"

namespace ho {

#ifdef THREAD_ENABLED
    class Thread {
       public:
        using Entry = void (*)(void*);

#if defined(__cpp_lib_hardware_interference_size)
        static constexpr size_t CACHE_LINE_BYTES = std::hardware_destructive_interference_size;
#else
        static constexpr size_t CACHE_LINE_BYTES = 128;
#endif

        ALWAYS_INLINE explicit Thread(Entry entry, void* userdata = nullptr) : thread_([=] { entry(userdata); }) {}

        Thread(const Thread&) = delete;
        Thread& operator=(const Thread&) = delete;

        ~Thread();

        void SetName(const std::string& name);
        static void SetCurrentName(const std::string& name);

        void Join();

        ALWAYS_INLINE static void Yield() { std::this_thread::yield(); }
        ALWAYS_INLINE static void Sleep(uint64_t ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }

       private:
        std::thread thread_;
    };

#else

    class Thread {
       public:
        using Entry = void (*)(void*);

        static constexpr size_t CACHE_LINE_BYTES = sizeof(void*);

        ALWAYS_INLINE explicit Thread(Entry entry, void* userdata = nullptr) { entry(userdata); }

        Thread(const Thread&) = delete;
        Thread& operator=(const Thread&) = delete;

        ~Thread() = default;

        ALWAYS_INLINE void SetName(const std::string&) {}
        ALWAYS_INLINE static void SetCurrentName(const std::string&) {}

        ALWAYS_INLINE void Join() {}

        ALWAYS_INLINE static void Yield() {}

        ALWAYS_INLINE static void Sleep(uint64_t) {}
    };

#endif  // THREAD_ENABLED

}  // namespace ho

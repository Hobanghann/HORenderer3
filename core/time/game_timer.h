#include <chrono>
#include <cstdint>

#include "core/macros.h"
#include "core/math/math_defs.h"

using ChronoClock = std::chrono::steady_clock;
using Duration = std::chrono::duration<int64_t, std::nano>;
using TimePoint = std::chrono::time_point<ChronoClock>;

class GameTimer {
   public:
    _ALWAYS_INLINE_ GameTimer();

    _ALWAYS_INLINE_ void Tick();
    _ALWAYS_INLINE_ float DeltaTime();
    _ALWAYS_INLINE_ float TotalTime();

    _ALWAYS_INLINE_ void Reset();
    _ALWAYS_INLINE_ void Stop();
    _ALWAYS_INLINE_ void Start();

   private:
    Duration delta_time_;
    Duration paused_time_;

    TimePoint base_time_;
    TimePoint curr_frame_time_;
    TimePoint prev_frame_time_;

    TimePoint stop_time_;

    bool is_stopped_;
};

GameTimer::GameTimer()
    : delta_time_(Duration(-1)),
      paused_time_(Duration(0)),
      base_time_(),
      curr_frame_time_(),
      prev_frame_time_(),
      stop_time_(),
      is_stopped_(false) {}

void GameTimer::Tick() {
    if (is_stopped_) {
        delta_time_ = Duration(0);
        return;
    }

    curr_frame_time_ = ChronoClock::now();

    delta_time_ = curr_frame_time_ - prev_frame_time_;
    prev_frame_time_ = curr_frame_time_;

    if (delta_time_ < Duration(0)) {
        delta_time_ = Duration(0);
    }
}

real GameTimer::DeltaTime() { return std::chrono::duration<real>(delta_time_).count(); }

real GameTimer::TotalTime() {
    if (is_stopped_) {
        Duration total = (stop_time_ - base_time_) - paused_time_;
        return std::chrono::duration<real>(total).count();
    }
    TimePoint curr_time = ChronoClock::now();
    Duration total = (curr_time - base_time_) - paused_time_;

    return std::chrono::duration<real>(total).count();
}

void GameTimer::Reset() {
    TimePoint curr_time = ChronoClock::now();

    base_time_ = curr_time;
    prev_frame_time_ = curr_time;
    stop_time_ = TimePoint();

    paused_time_ = Duration(0);
    delta_time_ = Duration(0);

    is_stopped_ = false;
}

void GameTimer::Stop() {
    if (!is_stopped_) {
        stop_time_ = ChronoClock::now();
        is_stopped_ = true;
    }
}

void GameTimer::Start() {
    if (is_stopped_) {
        TimePoint curr_time = ChronoClock::now();

        paused_time_ += curr_time - stop_time_;

        prev_frame_time_ = curr_time;
        stop_time_ = TimePoint();

        is_stopped_ = false;

        delta_time_ = Duration(0);
    }
}

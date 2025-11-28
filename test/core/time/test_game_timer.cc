#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "core/time/game_timer.h"

using namespace std::chrono_literals;

class GameTimerTest : public ::testing::Test {
 protected:
  void SetUp() override { timer.Reset(); }
  GameTimer timer;
};

TEST_F(GameTimerTest, Reset) {
  EXPECT_FLOAT_EQ(timer.DeltaTime(), 0.0_r);

  EXPECT_NEAR(timer.TotalTime(), 0.0_r, 0.0001f);
}

TEST_F(GameTimerTest, Tick) {
  using namespace std::chrono_literals;

  std::this_thread::sleep_for(1ms);
  timer.Tick();
  real dt = timer.DeltaTime();

  EXPECT_GT(dt, 0.0_r);
  EXPECT_LT(dt, 0.080_r);
}

TEST_F(GameTimerTest, Stop) {
  std::this_thread::sleep_for(1ms);
  timer.Tick();
  real before = timer.DeltaTime();

  timer.Stop();
  std::this_thread::sleep_for(2ms);
  timer.Tick();

  EXPECT_GT(before, 0.0_r);
  EXPECT_FLOAT_EQ(timer.DeltaTime(), 0.0_r);
}

TEST_F(GameTimerTest, Start) {
  std::this_thread::sleep_for(2ms);
  timer.Tick();
  real t1 = timer.TotalTime();

  timer.Stop();
  std::this_thread::sleep_for(5ms);

  timer.Start();
  timer.Tick();

  real t2 = timer.TotalTime();

  EXPECT_NEAR(t2, t1 + 0.005_r, 0.020_r);
}

TEST_F(GameTimerTest, SequentialTicks) {
  timer.Tick();

  std::this_thread::sleep_for(1ms);
  timer.Tick();
  real dt1 = timer.DeltaTime();

  std::this_thread::sleep_for(1ms);
  timer.Tick();
  real dt2 = timer.DeltaTime();

  EXPECT_GT(dt1, 0.0_r);
  EXPECT_GT(dt2, 0.0_r);

  EXPECT_LT(dt1, 0.020_r);
  EXPECT_LT(dt2, 0.020_r);
}

TEST_F(GameTimerTest, MultiplePauses) {
  std::this_thread::sleep_for(2ms);
  timer.Tick();
  real t0 = timer.TotalTime();

  timer.Stop();
  std::this_thread::sleep_for(3ms);
  timer.Start();

  timer.Tick();
  real t1 = timer.TotalTime();

  timer.Stop();
  std::this_thread::sleep_for(4ms);
  timer.Start();

  timer.Tick();
  real t2 = timer.TotalTime();

  EXPECT_GT(t1, t0);
  EXPECT_GT(t2, t1);
}

TEST_F(GameTimerTest, StopStartTwice) {
  std::this_thread::sleep_for(2ms);
  timer.Tick();
  real t0 = timer.TotalTime();

  // First Stop
  timer.Stop();
  timer.Tick();
  real t1 = timer.TotalTime();
  std::this_thread::sleep_for(3ms);

  // Second Stop: should do nothing
  timer.Stop();
  timer.Tick();
  real t2 = timer.TotalTime();
  std::this_thread::sleep_for(3ms);

  // Start again
  timer.Start();
  timer.Tick();
  real t3 = timer.TotalTime();

  EXPECT_GT(t1, t0);
  EXPECT_FLOAT_EQ(t1, t2);
  EXPECT_GT(t3, t2);
}

TEST_F(GameTimerTest, StartWithoutStop) {
  std::this_thread::sleep_for(2ms);
  timer.Tick();
  real t0 = timer.TotalTime();

  timer.Start();

  std::this_thread::sleep_for(1ms);
  timer.Tick();
  real t1 = timer.TotalTime();

  EXPECT_GT(t1, t0);
  EXPECT_NEAR(t1, t0 + 0.005_r, 0.015_r);
}

TEST_F(GameTimerTest, StopWithoutStart) {
  std::this_thread::sleep_for(20ms);
  timer.Tick();
  real t0 = timer.TotalTime();

  timer.Stop();

  std::this_thread::sleep_for(10ms);
  timer.Tick();
  real t1 = timer.TotalTime();

  EXPECT_GT(t1, t0);
  EXPECT_NEAR(t1, t0 + 0.010_r, 0.020_r);
}

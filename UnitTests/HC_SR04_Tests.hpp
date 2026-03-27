#include <gtest/gtest.h>
#include "../Devices/HC_SR04.hpp"
#include "../Peripherals/Timer/TimerBase.hpp"

struct FakeInputCapture
{
    uint32_t value = 0;
    uint8_t readCounter = 0;
    uint32_t Read() { readCounter++; return value; };
};

struct FakePwm
{
    Peripherals::PwmState state = Peripherals::PwmState::READY;
    bool startHasRun = false;
    void Start() { startHasRun = true; };
    Peripherals::PwmState GetState() const { return state; };
};

struct FakeTimer : public TimerBase<FakeTimer>
{
    FakeTimer(const uint32_t delay_)
        : TimerBase<FakeTimer>(delay_)
    {};

    uint32_t Now_Impl() const { return 30; }
};

TEST(HCSR04Tests, GetDistanceTest)
{
    FakeInputCapture rising;
    FakeInputCapture falling;
    FakePwm trigger;
    FakeTimer timer{ 50 };
    rising.value = 1000; //us
    falling.value = 2000; //us
    const float tempC = 20.f;
    static constexpr unsigned us_in_s = 1'000'000;
    static constexpr unsigned cm_in_m = 100;

    ASSERT_FALSE(trigger.startHasRun);
    Device::HC_SR04 hc_sr04{ rising, falling, trigger, timer };

    const float distance = hc_sr04.GetDistance(tempC); //temp [*C]

    const float expectedDistance = (falling.value - rising.value) * ( 331.8f + 0.6f * tempC ) / (2 * us_in_s) * cm_in_m;
    ASSERT_TRUE(trigger.startHasRun);
    ASSERT_GE(rising.readCounter, 1);
    ASSERT_GE(falling.readCounter, 1);
    ASSERT_NEAR(distance, expectedDistance, 0.01f);
};

#include <gtest/gtest.h>
#include "../Devices/HC_SR04.hpp"
#include "../Peripherals/Timer/TimerBase.hpp"

struct FakeInputCapture
{
    uint32_t value = 0;
    uint8_t readCounter = 0;
    uint32_t Read() { readCounter++; return value; };
    uint16_t GetMaxCounter() const { return static_cast<uint16_t>(999999); };
};

struct FakePwm
{
    Peripherals::PwmState state = Peripherals::PwmState::READY;
    bool startHasRun = false;
    bool stopHasRun = false;
    bool resetCounterHasRun = false;
    uint16_t counter = 21;
    void Start() { startHasRun = true; };
    void Stop() { stopHasRun = true; };
    void ResetCounter() { resetCounterHasRun = true; };
    uint16_t GetCounter() const { return counter; };
    uint16_t GetMaxCounter() const { return 20; };
    Peripherals::PwmState GetState() const { return state; };
};

class FakeTimer : public TimerBase<FakeTimer>
{
    uint32_t fakeNow = 30;

public:
    FakeTimer(const uint32_t delay_)
        : TimerBase<FakeTimer>(delay_)
    {};
    void SetFakeNow(const uint32_t fakeNow_) { fakeNow = fakeNow_; }
    uint32_t Now_Impl() const { return fakeNow; }
};

class HC_SR04Mock : public Device::HC_SR04<FakeInputCapture, FakePwm, FakeTimer>
{
public:
    HC_SR04Mock(FakeInputCapture& rising, FakeInputCapture& falling, FakePwm& trigger, FakeTimer& timer)
        : Device::HC_SR04<FakeInputCapture, FakePwm, FakeTimer>(rising, falling, trigger, timer)
    {};

    void SetFakeTimer(const FakeTimer& timer)
    {
        distanceMeasurementTimer = timer;
    }
};

struct HCSR04_Fixture : public ::testing::Test
{
    FakeInputCapture rising;
    FakeInputCapture falling;
    FakePwm trigger;
    FakeTimer timer{ 20 };
    const float tempC = 20.f;
    static constexpr unsigned us_in_s = 1'000'000;
    static constexpr unsigned cm_in_m = 100;

    HC_SR04Mock hc_sr04{ rising, falling, trigger, timer };

    void SetUp() override;
};

TEST_F(HCSR04_Fixture, GetDistanceTestReturnedValueWithoutTimeout)
{
    const float expectedDistance = (falling.value - rising.value) * ( 331.8f + 0.6f * tempC ) / (2 * us_in_s) * cm_in_m;
    std::optional<float> distance = std::nullopt;

    for(std::size_t i = 0; i < 3; i++)
        distance = hc_sr04.GetDistance(tempC);    

    ASSERT_TRUE(trigger.startHasRun);
    ASSERT_TRUE(trigger.stopHasRun);
    ASSERT_GE(trigger.counter, 20);
    ASSERT_TRUE(trigger.resetCounterHasRun);
    ASSERT_GE(rising.readCounter, 1);
    ASSERT_GE(falling.readCounter, 1);
    ASSERT_NEAR(distance.value(), expectedDistance, 0.01f);
};

TEST_F(HCSR04_Fixture, GetDistanceTestTimedOut)
{
    std::optional<float> distance = std::nullopt;

    for(std::size_t i = 0; i < 3; i++)
    {
        if (i > 0)
            hc_sr04.SetFakeTimer(FakeTimer{ 1 }); // reset timer to simulate timeout
        distance = hc_sr04.GetDistance(tempC);
    }

    ASSERT_TRUE(trigger.startHasRun);
    ASSERT_TRUE(trigger.stopHasRun);
    ASSERT_GE(trigger.counter, 20);
    ASSERT_TRUE(trigger.resetCounterHasRun);
    ASSERT_FALSE(distance.has_value());
};

void HCSR04_Fixture::SetUp()
{
    rising.value = 1000; //us
    falling.value = 2000; //us
}
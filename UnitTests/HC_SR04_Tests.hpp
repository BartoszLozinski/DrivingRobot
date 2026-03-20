#include <gtest/gtest.h>
#include "../Devices/HC_SR04/HC_SR04.hpp"

struct FakeInputCapture
{
    uint32_t value = 0;
    uint32_t Read() { return value; };
};


TEST(HCSR04Tests, GetDistanceTest)
{
    FakeInputCapture rising;
    FakeInputCapture falling;
    rising.value = 1000; //us
    falling.value = 2000; //us
    const float tempC = 20.f;
    static constexpr unsigned us_in_s = 1'000'000;
    static constexpr unsigned cm_in_m = 100;
    Device::HC_SR04<FakeInputCapture> hc_sr04{ rising, falling };

    const float distance = hc_sr04.GetDistance(tempC); //temp [*C]

    const float expectedDistance = (falling.value - rising.value) * ( 331.8f + 0.6f * tempC ) / (2 * us_in_s) * cm_in_m;
    ASSERT_NEAR(distance, expectedDistance, 0.01f);
};

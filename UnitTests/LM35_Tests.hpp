#pragma once

#include <gtest/gtest.h>
#include "../Devices/LM35.hpp"

struct FakeAdc
{
    float voltage = 0.213f;
    float ReadVoltage() const { return voltage; }
};

TEST(LM35Test, ReadTempCTest)
{
    FakeAdc adc{ 0.213f };
    Device::LM35 lm35{ adc };
    static constexpr float expectedTemp = 21.3f;

    const auto resultTemp = lm35.ReadTempC();

    ASSERT_NEAR(resultTemp, expectedTemp, 0.001f);
};
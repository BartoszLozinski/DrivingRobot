#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Peripherals/Timer/Interfaces/IPwm.hpp"

struct IPwmMock : public Peripherals::IPwm
{
    MOCK_METHOD(uint16_t, GetCounter, (), (const, override));
    MOCK_METHOD(uint16_t, GetMaxCounter, (), (const, override));
    MOCK_METHOD(uint32_t, GetPulse, (), (const, override));
    MOCK_METHOD(Peripherals::PwmState, GetState, (), (const, override));
    MOCK_METHOD(void, ResetCounter, (), (override));
    MOCK_METHOD(void, Start, (), (override));
    MOCK_METHOD(void, Stop, (), (override));
    MOCK_METHOD(void, SetPulse, (uint32_t pulse), (override));
    MOCK_METHOD(void, SetPulsePercent, (const uint8_t pulsePercent), (override));
};


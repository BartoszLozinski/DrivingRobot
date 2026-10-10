#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Peripherals/I2C/I2CBase.hpp"

struct I2CMock : public Peripherals::I2CBase
{
    MOCK_METHOD(Peripherals::I2CResult, Write, (const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer), (override));
    MOCK_METHOD(Peripherals::I2CResult, Read, (const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer), (override));
};

struct I2CMock_IT : public Peripherals::I2CBase_IT
{
    MOCK_METHOD(Peripherals::I2CResult, Write, (const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer), (override));
    MOCK_METHOD(Peripherals::I2CResult, Read, (const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer), (override));
    MOCK_METHOD(void, OnRxComplete, (), (override));
    MOCK_METHOD(void, OnTxComplete, (), (override));
    MOCK_METHOD(void, NotifyDataIsRead, (), (override));
    MOCK_METHOD(Peripherals::I2CState, GetState, (), (const, override));
};

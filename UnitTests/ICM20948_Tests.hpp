#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Mocks/I2CMock.hpp"

#include "Devices/ICM20948_Async.hpp"

class ICM20948Fixture : public ::testing::Test
{
public:
    I2CMock_IT i2cMockIt{};
    Device::ICM20948_Async icm20948{ i2cMockIt };

    static constexpr uint8_t DEV_ADDR = 0x69;
    static constexpr uint8_t ACCEL_XOUT_H = 0x2D;
};


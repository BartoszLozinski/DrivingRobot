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


TEST_F(ICM20948Fixture, ReadAccelerometerGyroDecodesSignedValues)
{
    std::span<uint8_t> pendingBuffer;
    // TODO: can this be wrote more readable like uint16_t and extract bits
    const std::array<uint8_t, 12> raw
    {
        0x40, 0x00,  // accel X:  16384 ->  1 g
        0xE0, 0x00,  // accel Y:  -8192 -> -0.5 g
        0x00, 0x00,  // accel Z:     0
        0x00, 0x83,  // gyro X:    131 ->  1 dps
        0xFF, 0x7D,  // gyro Y:   -131 -> -1 dps
        0x00, 0x00   // gyro Z:      0
    };

    EXPECT_CALL(i2cMockIt, Read(DEV_ADDR, ACCEL_XOUT_H, testing::_))
        .WillOnce([&](uint16_t, uint16_t, std::span<uint8_t> destination)
            {
                EXPECT_EQ(destination.size(), 12u);
                pendingBuffer = destination;
                return Peripherals::I2CResult::Success;
            }
        );

    EXPECT_FALSE(icm20948.ReadAccelerometerGyro().has_value());

    EXPECT_CALL(i2cMockIt, OnRxComplete())
        .WillOnce([&]()
            {
                std::ranges::copy(raw, pendingBuffer.begin());
            }
        );
    
    icm20948.OnRxComplete();
    EXPECT_CALL(i2cMockIt, NotifyDataIsRead()).Times(1);
    const auto sample = icm20948.ReadAccelerometerGyro();
    
    ASSERT_TRUE(sample.has_value());
    EXPECT_FLOAT_EQ(sample->accelX, 1.0f);
    EXPECT_FLOAT_EQ(sample->accelY, -0.5f);
    EXPECT_FLOAT_EQ(sample->gyroX, 1.0f);
    EXPECT_FLOAT_EQ(sample->gyroY, -1.0f);
}

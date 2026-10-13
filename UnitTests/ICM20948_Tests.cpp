#include "ICM20948_Tests.hpp"

TEST_F(ICM20948Fixture, ReadAccelerometerGyroDecodesSignedValues)
{
    std::span<uint8_t> pendingBuffer;
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

TEST_F(ICM20948Fixture, ReadWhoAmI)
{
    static constexpr uint8_t WHO_AM_I = 0x00;
    std::span<uint8_t> buffer{};
    const std::array<uint8_t, 1> expectedValue{ 0xEA };

    EXPECT_CALL(i2cMockIt, Read(DEV_ADDR, WHO_AM_I, testing::_))
        .WillOnce([&](uint16_t, uint16_t, std::span<uint8_t> regBuffer)
            {
                EXPECT_EQ(regBuffer.size(), 1);
                buffer = regBuffer;
                return Peripherals::I2CResult::Success;
            }
        );

    EXPECT_FALSE(icm20948.ReadWhoAmI().has_value());

    EXPECT_CALL(i2cMockIt, OnRxComplete())
        .WillOnce([&]()
            {
                std::ranges::copy(expectedValue, buffer.begin());
            }
        );;
    
    icm20948.OnRxComplete();

    EXPECT_CALL(i2cMockIt, NotifyDataIsRead()).Times(1);

    const auto result = icm20948.ReadWhoAmI();
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), expectedValue.at(0));
}

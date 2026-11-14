#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Devices/ICM20948_Async_Spi.hpp"

namespace
{
    class SpiMock : public Peripherals::ISpi_IT
    {
    public:
        MOCK_METHOD(Peripherals::SpiResult, Transmit, (std::span<uint8_t> data), (override));
        MOCK_METHOD(Peripherals::SpiResult, Receive, (std::span<uint8_t> buffer), (override));
        MOCK_METHOD(void, OnRxComplete, (), (override));
        MOCK_METHOD(void, OnTxComplete, (), (override));
        MOCK_METHOD(void, NotifyDataIsRead, (), (override));
    };

    class GpioOutputMock : public Peripherals::IGpioOutput
    {
    public:
        MOCK_METHOD(void, Set, (), (override));
        MOCK_METHOD(void, Clear, (), (override));
        MOCK_METHOD(void, Toggle, (), (override));
        MOCK_METHOD(Peripherals::GpioState, GetState, (), (const, override));
    };
}

TEST(ICM20948Spi, InitHandlesTxCompletionBeforeTransmitReturns)
{
    SpiMock spi;
    GpioOutputMock chipSelect;
    Device::ICM20948_Async_Spi icm20948{ spi, chipSelect };

    EXPECT_CALL(spi, Transmit(testing::_))
        .WillOnce([&icm20948](std::span<uint8_t> data) -> Peripherals::SpiResult
            {
                EXPECT_EQ(data.size(), 2u);
                EXPECT_EQ(data[0], 0x7Fu);
                EXPECT_EQ(data[1], 0x00u);
                icm20948.OnTxComplete();
                return Peripherals::SpiResult::Success;
            })
        .WillOnce([&icm20948](std::span<uint8_t> data) -> Peripherals::SpiResult
            {
                EXPECT_EQ(data.size(), 2u);
                EXPECT_EQ(data[0], 0x06u);
                EXPECT_EQ(data[1], 0x01u);
                icm20948.OnTxComplete();
                return Peripherals::SpiResult::Success;
            });
    EXPECT_CALL(spi, OnTxComplete()).Times(2);
    EXPECT_CALL(chipSelect, Clear()).Times(2);
    EXPECT_CALL(chipSelect, Set()).Times(2);

    icm20948.Init(Device::ICM20948::AccelerometerSensitivity::g4,
                  Device::ICM20948::GyroscopeSensitivity::dps500);
    icm20948.Init(Device::ICM20948::AccelerometerSensitivity::g4,
                  Device::ICM20948::GyroscopeSensitivity::dps500);
}

TEST(ICM20948SpiRead, ReadAccelerometerGyroDecodesSignedValues)
{
    SpiMock spi;
    GpioOutputMock chipSelect;
    Device::ICM20948_Async_Spi icm20948{ spi, chipSelect };

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

    //static constexpr uint8_t READ_BIT_MASK = 0x80;
    //static constexpr uint8_t ACCEL_XOUT_H = 0x2D;

    // Trigger the read command

    // uint8_t readCommand = static_cast<uint8_t>(ACCEL_XOUT_H) | READ_BIT_MASK;

    EXPECT_CALL(spi, Transmit(/*std::span<uint8_t>(&readCommand, sizeof(readCommand))*/ testing::_ ))
        .WillOnce([&](std::span<uint8_t> data) -> Peripherals::SpiResult
            {
                EXPECT_EQ(data.size(), 1u);
                EXPECT_EQ(data[0], 0xADu);
                icm20948.OnTxComplete();
                return Peripherals::SpiResult::Success;
            }
        );

    EXPECT_CALL(chipSelect, Clear()).Times(1);
    EXPECT_CALL(spi, OnTxComplete()).Times(1);
    
    EXPECT_FALSE(icm20948.ReadAccelerometerGyro().has_value());

    // Read the sample

    EXPECT_CALL(spi, Receive(testing::_))
        .WillOnce([&](std::span<uint8_t> destination)
            {
                EXPECT_EQ(destination.size(), 12u);
                pendingBuffer = destination;
                icm20948.OnRxComplete();
                return Peripherals::SpiResult::Success;
            }
        );


    EXPECT_CALL(spi, OnRxComplete())
        .WillOnce([&]()
            {
                std::ranges::copy(raw, pendingBuffer.begin());
            }
        );

    EXPECT_CALL(chipSelect, Set()).Times(1);
    ASSERT_FALSE(icm20948.ReadAccelerometerGyro().has_value());

    //Read sample from buffer

    EXPECT_CALL(spi, NotifyDataIsRead()).Times(1);
    const auto sample = icm20948.ReadAccelerometerGyro();

    ASSERT_TRUE(sample.has_value());
    ASSERT_FLOAT_EQ(sample->accelX, 1.0f);
    ASSERT_FLOAT_EQ(sample->accelY, -0.5f);
    ASSERT_FLOAT_EQ(sample->gyroX, 1.0f);
    ASSERT_FLOAT_EQ(sample->gyroY, -1.0f);
}

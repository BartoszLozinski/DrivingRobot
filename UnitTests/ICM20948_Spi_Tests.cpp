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

#include "LPS25HB_Tests.hpp"

TEST_F(LPS25HBFixture, ReadTemperature_Success)
{
    static constexpr uint16_t tempOutLowReg = 0x2B;
    
    uint8_t mockBuffer[2] = { 0x00, 0x00 };
    EXPECT_CALL(i2cMock, Read(devAddr, tempOutLowReg | AUTO_INCREMENT,
                              testing::Truly([&](std::span<uint8_t> s) {
                                  return s.size() == 2 && s[0] == mockBuffer[0] && s[1] == mockBuffer[1];
                              })))
        .WillOnce(testing::Return(Peripherals::I2CResult::Success));
    ASSERT_TRUE(lps25hb.ReadTemperature().has_value());
}

TEST_F(LPS25HBFixture, ReadPressure_Success)
{
    static constexpr uint16_t pressOutXLReg = 0x28;
    uint8_t mockBuffer[3] = { 0, 0, 0};
    EXPECT_CALL(i2cMock, Read(devAddr, pressOutXLReg | AUTO_INCREMENT,
                              testing::Truly([&](std::span<uint8_t> s) { //check span shape
                                    return s.size() == 3 &&
                                           s[0] == mockBuffer[0] &&
                                           s[1] == mockBuffer[1] &&
                                           s[2] == mockBuffer[2];
                                })))
                .WillOnce(testing::Return(Peripherals::I2CResult::Success));

    ASSERT_TRUE(lps25hb.ReadPressure().has_value());
}

TEST_F(LPS25HBAsyncFixture, ReadTemperatureAsync_Success)
{
    static constexpr uint16_t tempOutLowReg = 0x2B;

    EXPECT_CALL(i2cMock_IT, Read(devAddr, tempOutLowReg | AUTO_INCREMENT, testing::_))
        .WillOnce(testing::Return(Peripherals::I2CResult::Success));

    ASSERT_TRUE(lps25hbAsync.ReadTemperature() == std::nullopt);

    //Interrupt handling
    EXPECT_CALL(i2cMock_IT, OnRxComplete()).Times(1);
    lps25hbAsync.OnRxComplete();

    EXPECT_CALL(i2cMock_IT, NotifyDataIsRead()).Times(1);
    const auto resultTemp = lps25hbAsync.ReadTemperature();
    ASSERT_TRUE(resultTemp.has_value());
}

TEST_F(LPS25HBAsyncFixture, ReadTemperatureAsync_Busy)
{
    static constexpr uint16_t tempOutLowReg = 0x2B;

    EXPECT_CALL(i2cMock_IT, Read(devAddr, tempOutLowReg | AUTO_INCREMENT, testing::_))
        .WillOnce(testing::Return(Peripherals::I2CResult::Busy));

    ASSERT_TRUE(lps25hbAsync.ReadTemperature() == std::nullopt);
}

TEST_F(LPS25HBAsyncFixture, ReadTemperatureAsync_Error)
{
    static constexpr uint16_t tempOutLowReg = 0x2B;

    EXPECT_CALL(i2cMock_IT, Read(devAddr, tempOutLowReg | AUTO_INCREMENT, testing::_))
        .WillOnce(testing::Return(Peripherals::I2CResult::Error));

    ASSERT_TRUE(lps25hbAsync.ReadTemperature() == std::nullopt);
}

TEST_F(LPS25HBAsyncFixture, ReadPressureAsync_Success)
{
    static constexpr uint16_t pressOutXLReg = 0x28;

    EXPECT_CALL(i2cMock_IT, Read(devAddr, pressOutXLReg | AUTO_INCREMENT, testing::_))
        .WillOnce(testing::Return(Peripherals::I2CResult::Success));        

    ASSERT_TRUE(lps25hbAsync.ReadPressure() == std::nullopt);

    //Interrupt handling
    EXPECT_CALL(i2cMock_IT, OnRxComplete()).Times(1);
    lps25hbAsync.OnRxComplete();

    EXPECT_CALL(i2cMock_IT, NotifyDataIsRead()).Times(1);
    const auto resultPress = lps25hbAsync.ReadPressure();
    ASSERT_TRUE(resultPress.has_value());
}

TEST_F(LPS25HBAsyncFixture, ReadPressureAsync_Busy)
{
    static constexpr uint16_t pressOutXLReg = 0x28;

    EXPECT_CALL(i2cMock_IT, Read(devAddr, pressOutXLReg | AUTO_INCREMENT, testing::_))
        .WillOnce(testing::Return(Peripherals::I2CResult::Busy));

    ASSERT_TRUE(lps25hbAsync.ReadPressure() == std::nullopt);
}

TEST_F(LPS25HBAsyncFixture, ReadPressureAsync_Error)
{
    static constexpr uint16_t pressOutXLReg = 0x28;

    EXPECT_CALL(i2cMock_IT, Read(devAddr, pressOutXLReg | AUTO_INCREMENT, testing::_))
        .WillOnce(testing::Return(Peripherals::I2CResult::Error));

    ASSERT_TRUE(lps25hbAsync.ReadPressure() == std::nullopt);
}

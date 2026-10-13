#pragma once

#include "Devices/LPS25HB.hpp"
#include "Devices/LPS25HB_Async.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Mocks/I2CMock.hpp"

struct LPS25HBRegisters
{
    static constexpr uint16_t devAddr = 0x5D;
    static constexpr uint16_t AUTO_INCREMENT = 0x80;
};

class LPS25HBFixture : public ::testing::Test
                     , public LPS25HBRegisters
{
public:
    I2CMock i2cMock;
    Device::LPS25HB lps25hb{ i2cMock };
};

class LPS25HBAsyncFixture : public ::testing::Test
                          , public LPS25HBRegisters
{
public:
    I2CMock_IT i2cMock_IT;
    Device::LPS25HB_Async lps25hbAsync{ i2cMock_IT };
};

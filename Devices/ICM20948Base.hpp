#pragma once

#include <cstdint>
#include <optional>

namespace Device
{
    class ICM20948Base
    {
    protected:
        // documentation page 38/89
        // User Bank 0 Registers
        enum class RegisterAddresses : uint8_t
        {
            ADDR = 0x69, // 7-bit I2C address (AD0 high)
            WHO_AM_I = 0x00, // Value for ICM-20948 is 0xEA
            USER_CTRL = 0x03,
            LP_CONFIG = 0x05,
            PWR_MGMT_1 = 0x06,
            PWR_MGMT_2 = 0x07,
            // skipped registers, which are not currently used
            ACCEL_XOUT_H = 0x2D,
            ACCEL_XOUT_L = 0x2E,
            ACCEL_YOUT_H = 0x2F,
            ACCEL_YOUT_L = 0x30,
            ACCEL_ZOUT_H = 0x31,
            ACCEL_ZOUT_L = 0x32,
            GYRO_XOUT_H = 0x33,
            GYRO_XOUT_L = 0x34,
            GYRO_YOUT_H = 0x35,
            GYRO_YOUT_L = 0x36,
            GYRO_ZOUT_H = 0x37,
            GYRO_ZOUT_L = 0x38,
        };

    public:
        virtual ~ICM20948Base() = default;
        virtual void WakeUp() = 0;
    };
};

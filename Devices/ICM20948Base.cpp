#include "ICM20948Base.hpp"

namespace Device
{
    AccelerometerGyroSample ICM20948Base::ConvertRawSampleToPhysicalValues() const
    {
        const auto accelerometerSensitivity = GetAccelerometerSensitivity();
        const auto gyroSensitivity = GetGyroscopeSensitivity();

        // reading HIGH/LOW bytes and converting to signed 16-bit integers, then scaling
        return AccelerometerGyroSample
        {
            .accelX = static_cast<float>(static_cast<int16_t>((rawAccelerometerGyroBuffer[0] << 8) | rawAccelerometerGyroBuffer[1])) / accelerometerSensitivity,
            .accelY = static_cast<float>(static_cast<int16_t>((rawAccelerometerGyroBuffer[2] << 8) | rawAccelerometerGyroBuffer[3])) / accelerometerSensitivity,
            .accelZ = static_cast<float>(static_cast<int16_t>((rawAccelerometerGyroBuffer[4] << 8) | rawAccelerometerGyroBuffer[5])) / accelerometerSensitivity,
            .gyroX = static_cast<float>(static_cast<int16_t>((rawAccelerometerGyroBuffer[6] << 8) | rawAccelerometerGyroBuffer[7])) / gyroSensitivity,
            .gyroY = static_cast<float>(static_cast<int16_t>((rawAccelerometerGyroBuffer[8] << 8) | rawAccelerometerGyroBuffer[9])) / gyroSensitivity,
            .gyroZ = static_cast<float>(static_cast<int16_t>((rawAccelerometerGyroBuffer[10] << 8) | rawAccelerometerGyroBuffer[11])) / gyroSensitivity,
        };
    }

    float ICM20948Base::GetAccelerometerSensitivity() const
    {
        // documentation chapter 3.2 ACCEL_FS
        // page 12/89
        switch(accSensitivity)
        {
        using enum ICM20948::AccelerometerSensitivity;
        case g2:
            return 16384.f; // LSB/g
        case g4:
            return 8192.f;
        case g8:
            return 4096.f;
        case g16:
            return 2048.f;
        default:
            return 16384.f;
        };
    };

    float ICM20948Base::GetGyroscopeSensitivity() const
    {
        // documentation chapter 3.1 GYRO_FS
        // page 11/89
        switch(gyroSensitivity)
        {
        using enum ICM20948::GyroscopeSensitivity;
        case dps250:
            return 131.f; // LSB/dps
        case dps500:
            return 65.5f;
        case dps1000:
            return 32.8f;
        case dps2000:
            return 16.4f;
        default:
            return 131.f;
        };
    };

    bool ICM20948Base::IsReady() const
    {
        return isReady;
    }

    void ICM20948Base::InitIdle()
    {
        isReady = false;
        state = State::PreparingToWakeUp;
        SelectRegBank(RegBankSelBits::USER_BANK_0);
    }

    void ICM20948Base::InitWakeUpCompleted()
    {
        state = State::SettingRegBank;
        SelectRegBank(RegBankSelBits::USER_BANK_2);
    }

    void ICM20948Base::InitGyroSensitivityUpdated()
    {
        state = State::SettingRegBankToThePreviousValue;
        SelectRegBank(RegBankSelBits::USER_BANK_0);
    }

    void ICM20948Base::InitSettingRegBankToThePreviousValueDone()
    {
        state = State::Idle;
        isReady = true;
    }

    void ICM20948Base::WakeUpBase()
    {
        static constexpr uint8_t defaultPwrMgmt1Reg = 0x41; //default value after reset, device is in sleep mode (bit 7 = 1), auto selects best available clock source (bit 2:0 = 1-5)
        currentPwrMgmt1Reg = defaultPwrMgmt1Reg;

        static constexpr uint8_t SLEEP_MODE_BIT_MASK = 0x1 << 6; // 1 - sleep, 0 - wake up
        currentPwrMgmt1Reg &= ~SLEEP_MODE_BIT_MASK;

        state = State::WakeUpScheduled;
    }
}

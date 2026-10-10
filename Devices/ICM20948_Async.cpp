#include "ICM20948_Async.hpp"

namespace Device
{
    ICM20948_Async::ICM20948_Async(Peripherals::I2CBase_IT& i2c_)
        : i2c(i2c_) {}

    float ICM20948_Async::GetAccelerometerSensitivity() const
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

    float ICM20948_Async::GetGyroscopeSensitivity() const
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

    // TODO write tests
    void ICM20948_Async::Init(const ICM20948::AccelerometerSensitivity accSensitivity_, const ICM20948::GyroscopeSensitivity gyroSensitivity_)
    {
        switch (state)
        {
        using enum State;
        case Idle:
            isReady = false;
            SelectRegBank(RegBankSelBits::USER_BANK_0);
            state = PreparingToWakeUp;
            break;
        case ReadyToWakeUp:
            WakeUp();
            state = State::WakeUpScheduled;
            break;
        case WakeUpCompleted:
            SelectRegBank(RegBankSelBits::USER_BANK_2);
            state = State::SettingRegBank;
            break;
        case SettingRegBankDone:
            i2c.NotifyDataIsRead();
            StartRead( static_cast<uint16_t>(Bank2::RegisterAddresses::ACCEL_CONFIG)
                     , std::span<uint8_t>(&currentAccelConfig, sizeof(currentAccelConfig))
                     , AccelConfigReading);
            break;
        case AccelConfigReadyToRead:
            i2c.NotifyDataIsRead();
            SetSensitivity(accSensitivity_);
            state = State::SetAccelerometerSensitivity;
            break;
        case AccelerometerSensitivityUpdated:
            StartRead( static_cast<uint16_t>(Bank2::RegisterAddresses::GYRO_CONFIG_1)
                     , std::span<uint8_t>(&currentGyroConfig, sizeof(currentGyroConfig))
                     , GyroConfig1Reading);
            break;
        case GyroConfig1ReadyToRead:
            i2c.NotifyDataIsRead();
            SetSensitivity(gyroSensitivity_);
            state = State::SetGyroSensitivity;
            break;
        case GyroSensitivityUpdated:
            SelectRegBank(RegBankSelBits::USER_BANK_0);
            state = SettingRegBankToThePreviousValue;
            break;
        case SettingRegBankToThePreviousValueDone:
            state = Idle;
            isReady = true;
            break;
        default:
            break;
        };
    }
    
    bool ICM20948_Async::IsReady() const
    {
        return isReady;
    }

    void ICM20948_Async::OnRxComplete()
    {
        i2c.OnRxComplete();

        switch (state)
        {
        using enum State;
        
        case SampleReading:
            state = SampleReadyToRead;
            break;
        case AccelConfigReading:
            state = AccelConfigReadyToRead;
            break;
        case GyroConfig1Reading:
            state = GyroConfig1ReadyToRead;
            break;
        default:  
            state = RegisterReadyToRead;
            break;
        }
    }

    void ICM20948_Async::OnTxComplete()
    {
        i2c.OnTxComplete();
        i2c.NotifyDataIsRead();

        switch (state)
        {
        using enum State;
        case PreparingToWakeUp:
            state = ReadyToWakeUp;
            break;
        case WakeUpScheduled:
            state = WakeUpCompleted;
            break;
        case SettingRegBank:
            state = SettingRegBankDone;
            break;
        case SettingRegBankToThePreviousValue:
            state = SettingRegBankToThePreviousValueDone;
            break;
        case SetAccelerometerSensitivity:
            state = AccelerometerSensitivityUpdated;
            break;
        case SetGyroSensitivity:
            state = GyroSensitivityUpdated;
            break;
        default:
            break;
        }
    }

    // TODO write tests
    std::optional<AccelerometerGyroSample> ICM20948_Async::ReadAccelerometerGyro()
    {
        switch (state)
        {
        case State::Idle:
            StartRead(static_cast<uint16_t>(Bank0::RegisterAddresses::ACCEL_XOUT_H)
                    , rawAccelerometerGyroBuffer
                    , State::SampleReading);

            break;
        case State::SampleReadyToRead:
        {
            i2c.NotifyDataIsRead();
            state = State::Idle;
            // Documentation chapter 3.1/3.2 page 11/89
            const auto accelerometerSensitivity = GetAccelerometerSensitivity();
            const auto gyroSensitivity = GetGyroscopeSensitivity();

            // reading HIGH/LOW bytes and converting to signed 16-bit integers, then scaling to g's
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
        default:
            break;
        }

        return std::nullopt;
    }


    std::optional<uint8_t> ICM20948_Async::ReadRegister(const uint8_t reg)
    {
        if (state == State::Idle)
        {   
            state = i2c.Read(static_cast<uint16_t>(Bank0::RegisterAddresses::ADDR)
                            , reg
                            , std::span<uint8_t>(&readRegisterBuffer, sizeof(readRegisterBuffer))) == Peripherals::I2CResult::Success ? State::TransferScheduled : State::Error;
        }
        else if (state == State::RegisterReadyToRead)
        {
            i2c.NotifyDataIsRead();
            return readRegisterBuffer;
        }

        return std::nullopt;
    }

    std::optional<uint8_t> ICM20948_Async::ReadWhoAmI()
    {
        const auto result = ReadRegister(static_cast<uint8_t>(Bank0::RegisterAddresses::WHO_AM_I));
        if (result && state == State::RegisterReadyToRead)
        {
            state = State::Idle;
            return result;
        }

        return std::nullopt;
    }

    void ICM20948_Async::SelectRegBank(const RegBankSelBits regBank)
    {
        static constexpr uint8_t userBankBitsOffset = 4; //first 4 bits reserved

        uint8_t value = static_cast<uint8_t>(regBank) << userBankBitsOffset;
        i2c.Write(static_cast<uint16_t>(Bank0::RegisterAddresses::ADDR)
                , static_cast<uint16_t>(Bank0::RegisterAddresses::REG_BANK_SEL)
                , std::span<uint8_t>(&value, sizeof(value)));
    }

    void ICM20948_Async::StartRead(const uint16_t reg, std::span<uint8_t>(buffer), const State successfulState)
    {
        state = i2c.Read(static_cast<uint16_t>(Bank0::RegisterAddresses::ADDR)
                        , reg
                        , buffer) == Peripherals::I2CResult::Success ? successfulState : State::Error;
    }

    void ICM20948_Async::WakeUp()
    {
        static constexpr uint8_t defaultPwrMgmt1Reg = 0x41; //default value after reset, device is in sleep mode (bit 7 = 1), auto selects best available clock source (bit 2:0 = 1-5)
        currentPwrMgmt1Reg = defaultPwrMgmt1Reg;

        static constexpr uint8_t SLEEP_MODE_BIT_MASK = 0x1 << 6; // 1 - sleep, 0 - wake up
        currentPwrMgmt1Reg &= ~SLEEP_MODE_BIT_MASK;

        i2c.Write(static_cast<uint16_t>(Bank0::RegisterAddresses::ADDR)
                , static_cast<uint16_t>(Bank0::RegisterAddresses::PWR_MGMT_1)
                , std::span<uint8_t>(&currentPwrMgmt1Reg, sizeof(currentPwrMgmt1Reg)));
    }
};
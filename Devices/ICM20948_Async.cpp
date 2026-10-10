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


    void ICM20948_Async::Init([[maybe_unused]] const ICM20948::AccelerometerSensitivity accSensitivity)
    {
        switch (state)
        {
        using enum State;
        case Idle:
            isReady = false;
            i2c.NotifyDataIsRead();
            SelectRegBank(RegBankSelBits::USER_BANK_0);
            state = PreparingToWakeUp;
            break;
        case ReadyToWakeUp:
            i2c.NotifyDataIsRead();
            WakeUp();
            state = State::WakeUpScheduled;
            break;
        case WakeUpCompleted:
            // check if something needs to be adjusted
            i2c.NotifyDataIsRead();
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
            SetAccelerometerSensitivityValue(accSensitivity);
            state = State::SetAccelerometerSensitivity;
            break;
        case AccelerometerSensitivityUpdated:
        //in that step Gyro can be set up
            i2c.NotifyDataIsRead();
            SelectRegBank(RegBankSelBits::USER_BANK_0);
            state = SettingRegBankToThePreviousValue;
            break;
        case SettingRegBankToThePreviousValueDone:
            i2c.NotifyDataIsRead();
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
        default:  
            state = RegisterReadyToRead;
            break;
        }
    }

    void ICM20948_Async::OnTxComplete()
    {
        i2c.OnTxComplete();

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
        default:
            break;
        }
    }

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
            static constexpr float gyroSensitivity = 131.0f; // LSB/(°/s) for ±250°/s full scale range - TODO to be selected in a constructor and read
            AccelerometerGyroSample reading;

            // reading HIGH/LOW bytes and converting to signed 16-bit integers, then scaling to g's
            reading.accelX = static_cast<float>(static_cast<int16_t>((rawAccelerometerGyroBuffer[0] << 8) | rawAccelerometerGyroBuffer[1])) / accelerometerSensitivity;
            reading.accelY = static_cast<float>(static_cast<int16_t>((rawAccelerometerGyroBuffer[2] << 8) | rawAccelerometerGyroBuffer[3])) / accelerometerSensitivity;
            reading.accelZ = static_cast<float>(static_cast<int16_t>((rawAccelerometerGyroBuffer[4] << 8) | rawAccelerometerGyroBuffer[5])) / accelerometerSensitivity;
            reading.gyroX = static_cast<float>(static_cast<int16_t>((rawAccelerometerGyroBuffer[6] << 8) | rawAccelerometerGyroBuffer[7])) / gyroSensitivity;
            reading.gyroY = static_cast<float>(static_cast<int16_t>((rawAccelerometerGyroBuffer[8] << 8) | rawAccelerometerGyroBuffer[9])) / gyroSensitivity;
            reading.gyroZ = static_cast<float>(static_cast<int16_t>((rawAccelerometerGyroBuffer[10] << 8) | rawAccelerometerGyroBuffer[11])) / gyroSensitivity;
            
            return reading;
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

    void ICM20948_Async::SetAccelerometerSensitivityValue(const ICM20948::AccelerometerSensitivity accSensitivity_)
    {
        static constexpr uint8_t ACCEL_FS_SEL_MASK = 0b110; // sensitivity bits (documentation page 64/89 - chapter 10.15: ACCEL_CONFIG)
        static constexpr uint8_t ACCEL_FS_SEL_OFFSET = 1;
        this->accSensitivity = accSensitivity_;

        uint8_t value = currentAccelConfig;
        value &= ~(ACCEL_FS_SEL_MASK);
        value |= static_cast<uint8_t>(accSensitivity) << ACCEL_FS_SEL_OFFSET;
            
        i2c.Write(static_cast<uint16_t>(Bank0::RegisterAddresses::ADDR)
                , static_cast<uint16_t>(Bank2::RegisterAddresses::ACCEL_CONFIG)
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
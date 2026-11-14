#include "ICM20948_Async.hpp"

namespace Device
{
    ICM20948_Async::ICM20948_Async(Peripherals::I2CInterface_IT& i2c_)
        : i2c(i2c_) {}

    void ICM20948_Async::Init(const ICM20948::AccelerometerSensitivity accSensitivity_, const ICM20948::GyroscopeSensitivity gyroSensitivity_)
    {
        switch (state)
        {
        using enum State;
        case Idle:
            InitIdle();
            break;
        case ReadyToWakeUp:
            WakeUp();
            break;
        case WakeUpCompleted:
            InitWakeUpCompleted();
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
            break;
        case AccelerometerSensitivityUpdated:
            StartRead( static_cast<uint16_t>(Bank2::RegisterAddresses::GYRO_CONFIG_1)
                     , std::span<uint8_t>(&currentGyroConfig, sizeof(currentGyroConfig))
                     , GyroConfig1Reading);
            break;
        case GyroConfig1ReadyToRead:
            i2c.NotifyDataIsRead();
            SetSensitivity(gyroSensitivity_);
            break;
        case GyroSensitivityUpdated:
            InitGyroSensitivityUpdated();
            break;
        case SettingRegBankToThePreviousValueDone:
            InitSettingRegBankToThePreviousValueDone();
            break;
        default:
            break;
        };
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
            return ConvertRawSampleToPhysicalValues();
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
            state = State::Idle;
            return readRegisterBuffer;
        }

        return std::nullopt;
    }

    std::optional<uint8_t> ICM20948_Async::ReadWhoAmI()
    {
        const auto result = ReadRegister(static_cast<uint8_t>(Bank0::RegisterAddresses::WHO_AM_I));
        if (result)
            return result;

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
        WakeUpBase();
        
        i2c.Write(static_cast<uint16_t>(Bank0::RegisterAddresses::ADDR)
                , static_cast<uint16_t>(Bank0::RegisterAddresses::PWR_MGMT_1)
                , std::span<uint8_t>(&currentPwrMgmt1Reg, sizeof(currentPwrMgmt1Reg)));

    }
};
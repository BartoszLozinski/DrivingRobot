#include "ICM20948_Async_Spi.hpp"

namespace Device
{
    ICM20948_Async_Spi::ICM20948_Async_Spi(Peripherals::ISpi_IT& spi_, Peripherals::IGpioOutput& chipSelect)
        : spi(spi_)
        , chipSelect(chipSelect)
    {}

    void ICM20948_Async_Spi::Init(const ICM20948::AccelerometerSensitivity accSensitivity_, const ICM20948::GyroscopeSensitivity gyroSensitivity_)
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
            spi.NotifyDataIsRead();
            chipSelect.Clear();
            readCommand = static_cast<uint8_t>(Bank2::RegisterAddresses::ACCEL_CONFIG) | READ_BIT_MASK;
            state = State::SendCommandAccelConfigReading;
            if (spi.Transmit(std::span<uint8_t>(&readCommand, sizeof(readCommand))) != Peripherals::SpiResult::Success)
                state = State::Error;
            break;
        case ReadAccelConfig:
            state = State::AccelConfigReading;
            if (spi.Receive(std::span<uint8_t>(&currentAccelConfig, sizeof(currentAccelConfig))) != Peripherals::SpiResult::Success)
                state = State::Error;
            break;
        case AccelConfigReadyToRead:
            spi.NotifyDataIsRead();
            SetSensitivity(accSensitivity_);
            break;
        case AccelerometerSensitivityUpdated:
            chipSelect.Clear();
            readCommand = static_cast<uint8_t>(Bank2::RegisterAddresses::GYRO_CONFIG_1) | READ_BIT_MASK;
            state = State::SendCommandGyroConfig1Reading;
            if (spi.Transmit(std::span<uint8_t>(&readCommand, sizeof(readCommand))) != Peripherals::SpiResult::Success)
                state = State::Error;
            break;
        case ReadGyroConfig1:
            state = State::GyroConfig1Reading;
            if (spi.Receive(std::span<uint8_t>(&currentGyroConfig, sizeof(currentGyroConfig))) != Peripherals::SpiResult::Success)
                state = State::Error;
            break;
        case GyroConfig1ReadyToRead:
            spi.NotifyDataIsRead();
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

    void ICM20948_Async_Spi::OnRxComplete()
    {
        spi.OnRxComplete();
        chipSelect.Set();
        OnRxCompleteBase();
    }

    void ICM20948_Async_Spi::OnTxComplete()
    {
        spi.OnTxComplete();

        switch (state)
        {
        using enum State;
        case PreparingToWakeUp:
            chipSelect.Set();
            state = ReadyToWakeUp;
            break;
        case WakeUpScheduled:
            chipSelect.Set();
            state = WakeUpCompleted;
            break;
        case SettingRegBank:
            chipSelect.Set();
            state = SettingRegBankDone;
            break;
        case SettingRegBankToThePreviousValue:
            chipSelect.Set();
            state = SettingRegBankToThePreviousValueDone;
            break;
        case SetAccelerometerSensitivity:
            chipSelect.Set();
            state = AccelerometerSensitivityUpdated;
            break;
        case SetGyroSensitivity:
            chipSelect.Set();
            state = GyroSensitivityUpdated;
            break;
        case SendReadCommand:
            state = SendReadCommandDone;
            break;
        case SendCommandAccelConfigReading:
            state = ReadAccelConfig;
            break;
        case SendCommandGyroConfig1Reading:
            state = ReadGyroConfig1;
            break;
        case SendCommandReadAccelSample:
            state = ReadSample;
            break;
        default:
            break;
        }
    }

    std::optional<AccelerometerGyroSample> ICM20948_Async_Spi::ReadAccelerometerGyro()
    {
        switch (state)
        {
        case State::Idle:
            chipSelect.Clear();
            readCommand = static_cast<uint8_t>(Bank0::RegisterAddresses::ACCEL_XOUT_H) | READ_BIT_MASK;
            state = State::SendCommandReadAccelSample;
            if (spi.Transmit(std::span<uint8_t>(&readCommand, sizeof(readCommand))) != Peripherals::SpiResult::Success)
            {
                chipSelect.Set();
                state = State::Error;
            }
            break;
        case State::ReadSample:
            state = State::SampleReading;
            if (spi.Receive(rawAccelerometerGyroBuffer) != Peripherals::SpiResult::Success)
            {
                chipSelect.Set();
                state = State::Error;
            }
            break;
        case State::SampleReadyToRead:
        {
            spi.NotifyDataIsRead();
            state = State::Idle;
            return ConvertRawSampleToPhysicalValues();
        }
        default:
            break;
        }

        return std::nullopt;
    }

    std::optional<uint8_t> ICM20948_Async_Spi::ReadRegister(uint8_t reg)
    {
        if (state == State::Idle)
        {
            chipSelect.Clear();
            readCommand = reg | READ_BIT_MASK;
            state = State::SendReadCommand;
            if (spi.Transmit(std::span<uint8_t>(&readCommand, sizeof(readCommand))) != Peripherals::SpiResult::Success)
            {
                chipSelect.Set();
                state = State::Error;
            }
        }
        else if (state == State::SendReadCommandDone)
        {
            state = State::ReadingRegsiterScheduled;
            if (spi.Receive(std::span<uint8_t>(&readRegisterBuffer, sizeof(readRegisterBuffer))) != Peripherals::SpiResult::Success)
            {
                chipSelect.Set();
                state = State::Error;
            }
        }
        else if (state == State::RegisterReadyToRead)
        {
            chipSelect.Set();
            spi.NotifyDataIsRead();
            state = State::Idle;
            return readRegisterBuffer;
        }

        return std::nullopt;
    }

    std::optional<uint8_t> ICM20948_Async_Spi::ReadWhoAmI()
    {
        return ReadRegister(static_cast<uint8_t>(Bank0::RegisterAddresses::WHO_AM_I));
    }

    void ICM20948_Async_Spi::SelectRegBank(const RegBankSelBits regBank)
    {
        static constexpr uint8_t userBankBitsOffset = 4; //first 4 bits reserved
        uint8_t value = static_cast<uint8_t>(regBank) << userBankBitsOffset;
        txBuffer[0] = (static_cast<uint8_t>(Bank0::RegisterAddresses::REG_BANK_SEL) & WRITE_BIT_MASK);
        txBuffer[1] = value;

        chipSelect.Clear();
        const auto result = spi.Transmit(txBuffer);
        if (result != Peripherals::SpiResult::Success)
            chipSelect.Set();
    }

    void ICM20948_Async_Spi::WakeUp()
    {
        WakeUpBase();
        
        txBuffer[0] = (static_cast<uint8_t>(Bank0::RegisterAddresses::PWR_MGMT_1) & WRITE_BIT_MASK);
        txBuffer[1] = currentPwrMgmt1Reg;

        chipSelect.Clear();
        if (spi.Transmit(txBuffer) != Peripherals::SpiResult::Success)
        {
            chipSelect.Set();
            state = State::Error;
        }
    }
};
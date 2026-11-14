#include "ICM20948_Async_Spi.hpp"

namespace Device
{
    ICM20948_Async_Spi::ICM20948_Async_Spi(Peripherals::ISpi_IT& spi_, Peripherals::IGpioOutput& chipSelect)
        : spi(spi_)
        , chipSelect(chipSelect)
    {}

    float ICM20948_Async_Spi::GetAccelerometerSensitivity() const
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

    float ICM20948_Async_Spi::GetGyroscopeSensitivity() const
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

    void ICM20948_Async_Spi::Init(const ICM20948::AccelerometerSensitivity accSensitivity_, const ICM20948::GyroscopeSensitivity gyroSensitivity_)
    {
        switch (state)
        {
        using enum State;
        case Idle:
            isReady = false;
            state = PreparingToWakeUp;
            if (SelectRegBank(RegBankSelBits::USER_BANK_0) != Peripherals::SpiResult::Success)
                state = State::Error;
            break;
        case ReadyToWakeUp:
            WakeUp();
            break;
        case WakeUpCompleted:
            state = State::SettingRegBank;
            if (SelectRegBank(RegBankSelBits::USER_BANK_2) != Peripherals::SpiResult::Success)
                state = State::Error;
            break;
        case SettingRegBankDone:
            spi.NotifyDataIsRead();
            SetChipSelect(false);
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
            SetChipSelect(true);
            SetSensitivity(accSensitivity_);
            break;
        case AccelerometerSensitivityUpdated:
            SetChipSelect(false);
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
            SetChipSelect(true);
            SetSensitivity(gyroSensitivity_);
            break;
        case GyroSensitivityUpdated:
            state = SettingRegBankToThePreviousValue;
            if (SelectRegBank(RegBankSelBits::USER_BANK_0) != Peripherals::SpiResult::Success)
                state = State::Error;
            break;
        case SettingRegBankToThePreviousValueDone:
            SetChipSelect(true);
            state = Idle;
            isReady = true;
            break;
        default:
            break;
        };
    }
    
    bool ICM20948_Async_Spi::IsReady() const
    {
        return isReady;
    }

    void ICM20948_Async_Spi::OnRxComplete()
    {
        spi.OnRxComplete();
        SetChipSelect(true);

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
        case ReadingRegsiterScheduled:
        case TransferScheduled:
            state = RegisterReadyToRead;
            break;
        default:
            state = RegisterReadyToRead;
            break;
        }
    }

    void ICM20948_Async_Spi::OnTxComplete()
    {
        spi.OnTxComplete();

        switch (state)
        {
        using enum State;
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
        case PreparingToWakeUp:
            SetChipSelect(true);
            state = ReadyToWakeUp;
            break;
        case WakeUpScheduled:
            SetChipSelect(true);
            state = WakeUpCompleted;
            break;
        case SettingRegBank:
            SetChipSelect(true);
            state = SettingRegBankDone;
            break;
        case SettingRegBankToThePreviousValue:
            SetChipSelect(true);
            state = SettingRegBankToThePreviousValueDone;
            break;
        case SetAccelerometerSensitivity:
            SetChipSelect(true);
            state = AccelerometerSensitivityUpdated;
            break;
        case SetGyroSensitivity:
            SetChipSelect(true);
            state = GyroSensitivityUpdated;
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
            SetChipSelect(false);
            readCommand = static_cast<uint8_t>(Bank0::RegisterAddresses::ACCEL_XOUT_H) | READ_BIT_MASK;
            state = State::SendCommandReadAccelSample;
            if (spi.Transmit(std::span<uint8_t>(&readCommand, sizeof(readCommand))) != Peripherals::SpiResult::Success)
            {
                SetChipSelect(true);
                state = State::Error;
            }
            break;
        case State::ReadSample:
            state = State::SampleReading;
            if (spi.Receive(rawAccelerometerGyroBuffer) != Peripherals::SpiResult::Success)
            {
                SetChipSelect(true);
                state = State::Error;
            }
            break;
        case State::SampleReadyToRead:
        {
            spi.NotifyDataIsRead();
            state = State::Idle;
            // Documentation chapter 3.1/3.2 page 11/89
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
        default:
            break;
        }

        return std::nullopt;
    }

    std::optional<uint8_t> ICM20948_Async_Spi::ReadRegister(uint8_t reg)
    {
        if (state == State::Idle)
        {
            SetChipSelect(false);
            readCommand = reg | READ_BIT_MASK;
            state = State::SendReadCommand;
            if (spi.Transmit(std::span<uint8_t>(&readCommand, sizeof(readCommand))) != Peripherals::SpiResult::Success)
            {
                SetChipSelect(true);
                state = State::Error;
            }
        }
        else if (state == State::SendReadCommandDone)
        {
            state = State::ReadingRegsiterScheduled;
            if (spi.Receive(std::span<uint8_t>(&readRegisterBuffer, sizeof(readRegisterBuffer))) != Peripherals::SpiResult::Success)
            {
                SetChipSelect(true);
                state = State::Error;
            }
        }
        else if (state == State::RegisterReadyToRead)
        {
            SetChipSelect(true);
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

    Peripherals::SpiResult ICM20948_Async_Spi::SelectRegBank(const RegBankSelBits regBank)
    {
        static constexpr uint8_t userBankBitsOffset = 4; //first 4 bits reserved
        uint8_t value = static_cast<uint8_t>(regBank) << userBankBitsOffset;
        txBuffer[0] = (static_cast<uint8_t>(Bank0::RegisterAddresses::REG_BANK_SEL) & WRITE_BIT_MASK);
        txBuffer[1] = value;

        SetChipSelect(false);
        const auto result = spi.Transmit(txBuffer);
        if (result != Peripherals::SpiResult::Success)
            SetChipSelect(true);
        return result;
    }

    void ICM20948_Async_Spi::SetChipSelect(const bool state)
    {
        if (state)
            chipSelect.Set();
        else
            chipSelect.Clear();
    }

    void ICM20948_Async_Spi::WakeUp()
    {
        static constexpr uint8_t defaultPwrMgmt1Reg = 0x41; //default value after reset, device is in sleep mode (bit 7 = 1), auto selects best available clock source (bit 2:0 = 1-5)
        currentPwrMgmt1Reg = defaultPwrMgmt1Reg;

        static constexpr uint8_t SLEEP_MODE_BIT_MASK = 0x1 << 6; // 1 - sleep, 0 - wake up
        currentPwrMgmt1Reg &= ~SLEEP_MODE_BIT_MASK;

        txBuffer[0] = (static_cast<uint8_t>(Bank0::RegisterAddresses::PWR_MGMT_1) & WRITE_BIT_MASK);
        txBuffer[1] = currentPwrMgmt1Reg;

        SetChipSelect(false);
        state = State::WakeUpScheduled;
        if (spi.Transmit(txBuffer) != Peripherals::SpiResult::Success)
        {
            SetChipSelect(true);
            state = State::Error;
        }
    }
};
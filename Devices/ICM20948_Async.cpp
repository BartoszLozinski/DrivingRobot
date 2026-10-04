#include "ICM20948_Async.hpp"

namespace Device
{
    ICM20948_Async::ICM20948_Async(Peripherals::I2CBase_IT& i2c_)
        : i2c(i2c_) {}

    bool ICM20948_Async::IsAwake() const
    {
        return isAwake;
    }
    
    void ICM20948_Async::OnRxComplete()
    {
        i2c.OnRxComplete();

        using enum State;
        switch (state)
        {
        case SampleReading:
            state = SampleReadyToRead;
            break;
        default:  
            state = RegisterReadyToRead;
        }
    }

    void ICM20948_Async::OnTxComplete()
    {
        i2c.OnTxComplete();

        using enum State;
        if (state == WakeUpScheduled)
            state = WakeUpCompleted;
    }

    std::optional<AccelerometerGyroSample> ICM20948_Async::ReadAccelerometerGyro()
    {
        switch (state)
        {
        case State::Idle:
            StartRead(static_cast<uint16_t>(RegisterAddresses::ACCEL_XOUT_H)
                    , rawAccelerometerGyroBuffer
                    , State::SampleReading);

            break;
        case State::SampleReadyToRead:
        {
            i2c.NotifyDataIsRead();
            state = State::Idle;
            // Documentation chapter 3.1/3.2 page 11/89
            static constexpr float accelerometerSensitivity = 16384.0f; // LSB/g for ±2g full scale range - TODO to be selected in a constructor and read
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
            state = i2c.Read(static_cast<uint16_t>(RegisterAddresses::ADDR)
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
        const auto result = ReadRegister(static_cast<uint8_t>(RegisterAddresses::WHO_AM_I));
        if (result && state == State::RegisterReadyToRead)
        {
            state = State::Idle;
            return result;
        }

        return std::nullopt;
    }

    void ICM20948_Async::StartRead(const uint16_t reg, std::span<uint8_t>(buffer), const State successfulState)
    {
        state = i2c.Read(static_cast<uint16_t>(RegisterAddresses::ADDR)
                        , reg
                        , buffer) == Peripherals::I2CResult::Success ? successfulState : State::Error;
    }

    void ICM20948_Async::WakeUp()
    {
        if (isAwake)
            return;

        switch (state)
        {
        case State::Idle:
            StartRead( static_cast<uint16_t>(RegisterAddresses::PWR_MGMT_1)
                     , std::span<uint8_t>(&readRegisterBuffer, sizeof(readRegisterBuffer))
                     , State::TransferScheduled);
            break;
        case State::RegisterReadyToRead:
            static constexpr uint8_t defaultPwrMgmt1Reg = 0x41; //default value after reset, device is in sleep mode (bit 7 = 1), auto selects best available clock source (bit 2:0 = 1-5)
            currentPwrMgmt1Reg = defaultPwrMgmt1Reg;
            
            i2c.NotifyDataIsRead();
            state = State::WakeUpScheduled;
            static constexpr uint8_t SLEEP_MODE_BIT_MASK = 0x1 << 6; // 1 - sleep, 0 - wake up
            currentPwrMgmt1Reg &= ~SLEEP_MODE_BIT_MASK;

            i2c.Write(static_cast<uint16_t>(RegisterAddresses::ADDR)
                    , static_cast<uint16_t>(RegisterAddresses::PWR_MGMT_1)
                    , std::span<uint8_t>(&currentPwrMgmt1Reg, sizeof(currentPwrMgmt1Reg)));
            break;
        case State::WakeUpCompleted:
            // check if something needs to be adjusted
            i2c.NotifyDataIsRead();
            isAwake = true;
            state = State::Idle;
            break;
        default:
            break;
        };
    }
    
};
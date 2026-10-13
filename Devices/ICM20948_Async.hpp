#pragma once

#include "ICM20948Base.hpp"
#include "Peripherals/I2C/I2CBase.hpp"
#include "type_traits"

struct AccelerometerGyroSample
{
    float accelX{};
    float accelY{};
    float accelZ{};
    float gyroX{};
    float gyroY{};
    float gyroZ{};
};

// TODO add documentation

namespace Device
{
    namespace ICM20948
    {
        enum class AccelerometerSensitivity : uint8_t
        {
            g2 = 0b00,
            g4 = 0b01,
            g8 = 0b10,
            g16 = 0b11,
        };

        enum class GyroscopeSensitivity : uint8_t
        {
            dps250 = 0b00,
            dps500 = 0b01,
            dps1000 = 0b10,
            dps2000 = 0b11,
        };
    };

    class ICM20948_Async : public ICM20948Base
    {
        using AccelerometerGyroBuffer = std::array<uint8_t, 12>;

    private:
        enum class State
        {
            Idle,
            TransferScheduled,
            PreparingToWakeUp,
            ReadyToWakeUp,
            WakeUpScheduled,
            RegisterReadyToRead,
            SampleReading,
            SampleReadyToRead,
            SettingRegBank,
            SettingRegBankToThePreviousValue,
            SettingRegBankDone,
            SettingRegBankToThePreviousValueDone,
            AccelConfigReading,
            AccelConfigReadyToRead,
            GyroConfig1Reading,
            GyroConfig1ReadyToRead,
            SetAccelerometerSensitivity,
            AccelerometerSensitivityUpdated,
            SetGyroSensitivity,
            GyroSensitivityUpdated,
            WakeUpCompleted,
            Error,
        };

        enum class RegBankSelBits : uint8_t
        {
            USER_BANK_0 = 0b00,
            USER_BANK_1 = 0b01,
            USER_BANK_2 = 0b10,
            USER_BANK_3 = 0b11,
        };

        Peripherals::I2CBase_IT& i2c;
        State state = State::Idle;
        ICM20948::AccelerometerSensitivity accSensitivity = ICM20948::AccelerometerSensitivity::g2;
        ICM20948::GyroscopeSensitivity gyroSensitivity = ICM20948::GyroscopeSensitivity::dps250;
        bool isReady = false;

        uint8_t currentPwrMgmt1Reg = 0x41; //default value after reset, device is in sleep mode (bit 7 = 1), auto selects best available clock source (bit 2:0 = 1-5)
        uint8_t readRegisterBuffer{};
        uint8_t currentAccelConfig{};
        uint8_t currentGyroConfig{};
        AccelerometerGyroBuffer rawAccelerometerGyroBuffer{};

        [[nodiscard]] float GetAccelerometerSensitivity() const;
        [[nodiscard]] float GetGyroscopeSensitivity() const;

        [[nodiscard]] std::optional<uint8_t> ReadRegister(uint8_t reg);
        void StartRead(const uint16_t reg, std::span<uint8_t>(buffer), const State succesfulState);
        void SelectRegBank(const RegBankSelBits regBank);

        template<typename T>
            requires (std::is_same_v<T, ICM20948::AccelerometerSensitivity> || std::is_same_v<T, ICM20948::GyroscopeSensitivity>)
        void SetSensitivity(const T sensitivity)
        {
            static constexpr uint8_t SENS_FS_SEL_MASK = 0b110; // sensitivity bits (documentation page 59/89 - chapter 10.2/10.15: GYRO_CONFIG_1 or ACCEL_CONFIG)
            static constexpr uint8_t SENS_FS_SEL_OFFSET = 1;
            uint8_t value = 0;
            uint16_t memoryAddress = 0;

            if constexpr (std::is_same_v<T, ICM20948::AccelerometerSensitivity>)
            {
                accSensitivity = sensitivity;
                value = currentAccelConfig;
                memoryAddress = static_cast<uint16_t>(Bank2::RegisterAddresses::ACCEL_CONFIG);
            }
            else
            {
                gyroSensitivity = sensitivity;
                value = currentGyroConfig;
                memoryAddress = static_cast<uint16_t>(Bank2::RegisterAddresses::GYRO_CONFIG_1);
            }

            value &= ~(SENS_FS_SEL_MASK);
            value |= static_cast<uint8_t>(sensitivity) << SENS_FS_SEL_OFFSET;
                
            i2c.Write(static_cast<uint16_t>(Bank0::RegisterAddresses::ADDR)
                    , memoryAddress
                    , std::span<uint8_t>(&value, sizeof(value)));
        }

    public:
        ICM20948_Async(Peripherals::I2CBase_IT& i2c_);

        [[nodiscard]] bool IsReady() const;
        void WakeUp() override;
        void OnTxComplete();
        void OnRxComplete();
        std::optional<uint8_t> ReadWhoAmI();
        std::optional<AccelerometerGyroSample> ReadAccelerometerGyro();

        void Init(const ICM20948::AccelerometerSensitivity accSensitivity_, const ICM20948::GyroscopeSensitivity gyroSensitivity_);
    };
};
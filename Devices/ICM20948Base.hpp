#pragma once

#include <array>
#include <cstdint>
#include <optional>

struct AccelerometerGyroSample
{
    float accelX{};
    float accelY{};
    float accelZ{};
    float gyroX{};
    float gyroY{};
    float gyroZ{};
};


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

    class ICM20948Base
    {
    protected:
        // documentation page 38/89
        // User Bank 0 Registers
        using AccelerometerGyroBuffer = std::array<uint8_t, 12>;

        struct Bank0
        {
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
                REG_BANK_SEL = 0x7F, // Register to select user bank (0-3) same address for each bank
            };
        };

        struct Bank2
        {
            enum class RegisterAddresses : uint8_t
            {
                ACCEL_CONFIG = 0x14,
                ACCEL_CONFIG_2 = 0x15,
                GYRO_CONFIG_1 = 0x01,
                GYRO_CONFIG_2 = 0x02,
                REG_BANK_SEL = 0x7F, // Register to select user bank (0-3) same address for each bank
            };
        };

                enum class State
        {
            Idle,
            TransferScheduled,
            SendReadCommand,
            SendReadCommandDone,
            ReadingRegsiterScheduled,
            PreparingToWakeUp,
            ReadyToWakeUp,
            WakeUpScheduled,
            RegisterReadyToRead,
            SendCommandReadAccelSample,
            ReadSample,
            SampleReading,
            SampleReadyToRead,
            SettingRegBank,
            SettingRegBankToThePreviousValue,
            SettingRegBankDone,
            SettingRegBankToThePreviousValueDone,
            SendCommandAccelConfigReading,
            ReadAccelConfig,
            AccelConfigReading,
            AccelConfigReadyToRead,
            SendCommandGyroConfig1Reading,
            ReadGyroConfig1,
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

        State state = State::Idle;
        ICM20948::AccelerometerSensitivity accSensitivity = ICM20948::AccelerometerSensitivity::g2;
        ICM20948::GyroscopeSensitivity gyroSensitivity = ICM20948::GyroscopeSensitivity::dps250;
        bool isReady = false;

        uint8_t currentPwrMgmt1Reg = 0x41; //default value after reset, device is in sleep mode (bit 7 = 1), auto selects best available clock source (bit 2:0 = 1-5)
        uint8_t readRegisterBuffer{};
        uint8_t currentAccelConfig{};
        uint8_t currentGyroConfig{};
        AccelerometerGyroBuffer rawAccelerometerGyroBuffer{};

    protected:

        [[nodiscard]] float GetAccelerometerSensitivity() const;
        [[nodiscard]] float GetGyroscopeSensitivity() const;
        [[nodiscard]] virtual std::optional<uint8_t> ReadRegister(uint8_t reg) = 0;
        virtual void SelectRegBank(const RegBankSelBits regBank) = 0;

        template<typename T>
            requires (std::is_same_v<T, ICM20948::AccelerometerSensitivity> || std::is_same_v<T, ICM20948::GyroscopeSensitivity>)
        std::pair<uint8_t, uint8_t> SetSensitivityBase(const T sensitivity)
        {
            static constexpr uint8_t SENS_FS_SEL_MASK = 0b110; // sensitivity bits (documentation page 59/89 - chapter 10.2/10.15: GYRO_CONFIG_1 or ACCEL_CONFIG)
            static constexpr uint8_t SENS_FS_SEL_OFFSET = 1;
            uint8_t value = 0;
            uint8_t memoryAddress = 0;

            if constexpr (std::is_same_v<T, ICM20948::AccelerometerSensitivity>)
            {
                accSensitivity = sensitivity;
                value = currentAccelConfig;
                memoryAddress = static_cast<uint8_t>(Bank2::RegisterAddresses::ACCEL_CONFIG);
            }
            else
            {
                gyroSensitivity = sensitivity;
                value = currentGyroConfig;
                memoryAddress = static_cast<uint8_t>(Bank2::RegisterAddresses::GYRO_CONFIG_1);
            }

            value &= ~(SENS_FS_SEL_MASK);
            value |= static_cast<uint8_t>(sensitivity) << SENS_FS_SEL_OFFSET;
            
            if constexpr (std::is_same_v<T, ICM20948::AccelerometerSensitivity>)
                state = State::SetAccelerometerSensitivity;
            else
                state = State::SetGyroSensitivity;
            
            return { memoryAddress, value };
        }

        [[nodiscard]] AccelerometerGyroSample ConvertRawSampleToPhysicalValues() const;
        void InitIdle();
        void InitWakeUpCompleted();
        void InitGyroSensitivityUpdated();
        void InitSettingRegBankToThePreviousValueDone();
        void WakeUpBase();

    public:
        virtual ~ICM20948Base() = default;
        
        virtual void Init(const ICM20948::AccelerometerSensitivity accSensitivity_, const ICM20948::GyroscopeSensitivity gyroSensitivity_) = 0;
        [[nodiscard]] bool IsReady() const;
        virtual void OnRxComplete() = 0;
        virtual void OnTxComplete() = 0;
        virtual std::optional<AccelerometerGyroSample> ReadAccelerometerGyro() = 0;
        virtual std::optional<uint8_t> ReadWhoAmI() = 0;
        virtual void WakeUp() = 0;
    };
};

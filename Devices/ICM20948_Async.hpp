#pragma once

#include "ICM20948Base.hpp"
#include "Peripherals/I2C/I2CBase.hpp"

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
// TODO add sensitivity selection
// remember to select appropriate bank
// check before init
// select back to BANK0 when finished with other banks
// ADD init function

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
            WakeUpScheduled,
            RegisterReadyToRead,
            SampleReading,
            SampleReadyToRead,
            SettingRegBank,
            SettingRegBankToThePreviousValue,
            SettingRegBankDone,
            SettingRegBankToThePreviousValueDone,
            RegBankSelReading,
            RegBankSelReadyToRead,
            AccelConfigReading,
            AccelConfigReadyToRead,
            SetAccelerometerSensitivity,
            AccelerometerSensitivityUpdated,
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
        bool isAwake = false;
        //TODO add events beside states and maybe states per event with current event value
        bool isInitialized = false; // flag marking if sensitivity is already set up

        uint8_t currentPwrMgmt1Reg = 0x41; //default value after reset, device is in sleep mode (bit 7 = 1), auto selects best available clock source (bit 2:0 = 1-5)
        uint8_t readRegisterBuffer{};
        uint8_t currentRegBankSel{};
        uint8_t currentAccelConfig{};
        AccelerometerGyroBuffer rawAccelerometerGyroBuffer{};

        void StartRead(const uint16_t reg, std::span<uint8_t>(buffer), const State succesfulState);
        void SelectRegBank(const RegBankSelBits regBank);
        void SetAccelerometerSensitivityValue(const ICM20948::AccelerometerSensitivity accSensitivity);
        [[nodiscard]] std::optional<uint8_t> ReadRegister(uint8_t reg);

    public:
        ICM20948_Async(Peripherals::I2CBase_IT& i2c_);

        [[nodiscard]] bool IsAwake() const;
        [[nodiscard]] bool IsInitialized() const;
        void WakeUp() override;
        void OnTxComplete();
        void OnRxComplete();
        std::optional<uint8_t> ReadWhoAmI();
        std::optional<AccelerometerGyroSample> ReadAccelerometerGyro();
        void SetSensitivity(const ICM20948::AccelerometerSensitivity accSensitivity);

        void Init(const ICM20948::AccelerometerSensitivity accSensitivity);
    };
};
#pragma once

#include "ICM20948Base.hpp"
#include "../Peripherals/I2C/Interfaces/I2CInterface_IT.hpp"
#include "type_traits"
#include <optional>

namespace Device
{
    class ICM20948_Async : public ICM20948Base
    {
    private:
        Peripherals::I2CInterface_IT& i2c;

        void StartRead(const uint16_t reg, std::span<uint8_t>(buffer), const State succesfulState);

        template<typename T>
            requires (std::is_same_v<T, ICM20948::AccelerometerSensitivity> || std::is_same_v<T, ICM20948::GyroscopeSensitivity>)
        void SetSensitivity(const T sensitivity)
        {
            auto [memoryAddress, value] = SetSensitivityBase(sensitivity);

            i2c.Write(static_cast<uint16_t>(Bank0::RegisterAddresses::ADDR)
                    , memoryAddress
                    , std::span<uint8_t>(&value, sizeof(value)));
        }
        
    protected:
        [[nodiscard]] std::optional<uint8_t> ReadRegister(uint8_t reg) override final;
        void SelectRegBank(const RegBankSelBits regBank) override final;

    public:
        ICM20948_Async(Peripherals::I2CInterface_IT& i2c_);

        void Init(const ICM20948::AccelerometerSensitivity accSensitivity_, const ICM20948::GyroscopeSensitivity gyroSensitivity_) override final;
        void OnRxComplete() override final;
        void OnTxComplete() override final;
        [[nodiscard]] std::optional<AccelerometerGyroSample> ReadAccelerometerGyro() override final;
        [[nodiscard]] std::optional<uint8_t> ReadWhoAmI() override final;
        void WakeUp() override final;
    };
};
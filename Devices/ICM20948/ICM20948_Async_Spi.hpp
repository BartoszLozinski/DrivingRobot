#pragma once

#include "ICM20948Base.hpp"
#include "../Peripherals/SPI/Interfaces/ISpi_IT.hpp"
#include "../Peripherals/GPIO/Interfaces/IGpioOutput.hpp"
#include "type_traits"
#include <optional>

//TODO make common base for I2C and SPI
//TODO select different pin for SCK instead of PA5, as it lights LD@ by defualt wiht every tick

namespace Device
{
    class ICM20948_Async_Spi : public ICM20948Base
    {
    private:
        Peripherals::ISpi_IT& spi;
        Peripherals::IGpioOutput& chipSelect;

        static constexpr uint8_t READ_BIT_MASK = 0x80; //MSB 1
        static constexpr uint8_t WRITE_BIT_MASK = 0x7F; // MSB 0
        uint8_t readCommand{};
        std::array<uint8_t, 2> txBuffer{};

        template<typename T>
            requires (std::is_same_v<T, ICM20948::AccelerometerSensitivity> || std::is_same_v<T, ICM20948::GyroscopeSensitivity>)
        void SetSensitivity(const T sensitivity)
        {
            auto [memoryAddress, value] = SetSensitivityBase(sensitivity);

            txBuffer[0] = (static_cast<uint8_t>(memoryAddress) & WRITE_BIT_MASK);
            txBuffer[1] = value;
            if constexpr (std::is_same_v<T, ICM20948::AccelerometerSensitivity>)
                state = State::SetAccelerometerSensitivity;
            else
                state = State::SetGyroSensitivity;
            
            chipSelect.Clear();

            if (spi.Transmit(txBuffer) != Peripherals::SpiResult::Success)
            {
                chipSelect.Set();
                state = State::Error;
            }
        }

    protected:
        [[nodiscard]] std::optional<uint8_t> ReadRegister(uint8_t reg) override final;
        void SelectRegBank(const RegBankSelBits regBank) override final;

    public:
        ICM20948_Async_Spi(Peripherals::ISpi_IT& spi_, Peripherals::IGpioOutput& chipSelect);
        
        void Init(const ICM20948::AccelerometerSensitivity accSensitivity_, const ICM20948::GyroscopeSensitivity gyroSensitivity_) override final;
        void OnRxComplete() override final;
        void OnTxComplete() override final;
        [[nodiscard]] std::optional<AccelerometerGyroSample> ReadAccelerometerGyro() override final;
        [[nodiscard]] std::optional<uint8_t> ReadWhoAmI() override final;
        void WakeUp() override final;
    };
};
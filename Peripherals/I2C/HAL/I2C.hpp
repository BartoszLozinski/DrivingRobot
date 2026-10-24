#pragma once
#include "Interfaces/I2CInterface.hpp"

extern "C"
{
    #include "stm32l4xx_hal.h"
}

namespace Peripherals
{
    namespace HAL
    {
        class I2C : public I2CInterface
        {
        private:
            static constexpr uint8_t addressOffset = 1; // STM32 HAL library uses 8-bit address, so we need to shift the 7-bit address by 1 to the left

            I2C_HandleTypeDef& i2cHandle;
            uint32_t timeOut{ HAL_MAX_DELAY };

        public:
            I2CResult Write(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer) override;
            [[nodiscard]] I2CResult Read(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer) override;

            I2C() = delete;
            I2C(const I2C&) = delete;
            I2C(I2C&&) = delete;
            I2C& operator=(const I2C&) = delete;
            I2C& operator=(I2C&&) = delete;
            ~I2C() = default;
            explicit I2C(I2C_HandleTypeDef& i2cHandle_);
            void SetTimeout(const uint32_t timeOut_);
        };
    }
}
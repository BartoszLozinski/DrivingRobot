#pragma once

#include "Interfaces/I2CInterface_IT.hpp"
#include <array>
#include <optional>
#include <span>

extern "C"
{
    #include "stm32l4xx_hal.h"
}

namespace Peripherals
{
    namespace HAL
    {
        class I2C_IT : public I2CInterface_IT
        {
        private:
            static constexpr uint8_t addressOffset = 1; // STM32 HAL library uses 8-bit address, so we need to shift the 7-bit address by 1 to the left

            I2C_HandleTypeDef& i2cHandle;
            volatile I2CState state{  I2CState::Idle };

        public:
            I2C_IT() = delete;
            I2C_IT(const I2C_IT&) = delete;
            I2C_IT(I2C_IT&&) = delete;
            I2C_IT& operator=(const I2C_IT&) = delete;
            I2C_IT& operator=(I2C_IT&&) = delete;
            ~I2C_IT() = default;
            explicit I2C_IT(I2C_HandleTypeDef& i2cHandle_);

            [[nodiscard]] I2CResult Write(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer) override;
            [[nodiscard]] I2CResult Read(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer) override;

            void OnRxComplete() override;
            void OnTxComplete() override;
            void NotifyDataIsRead() override;
            Peripherals::I2CState GetState() const override { return state; }

        };
    }
}
#pragma once

#include "Interfaces/ISpi_IT.hpp"

extern "C"
{
#include "stm32l4xx_hal.h"
}

#if defined(HAL_SPI_MODULE_ENABLED)

namespace Peripherals::HAL
{
    class Spi_IT : public ISpi_IT
    {
    private:
        SPI_HandleTypeDef& spiHandle;
        SpiState state{ SpiState::Idle };

    public:
        SpiResult Transmit(std::span<uint8_t> data) override;
        SpiResult Receive(std::span<uint8_t> buffer) override;
        
        Spi_IT() = delete;
        Spi_IT(const Spi_IT&) = delete;
        Spi_IT(Spi_IT&&) = delete;
        Spi_IT& operator=(const Spi_IT&) = delete;
        Spi_IT& operator=(Spi_IT&&) = delete;
        ~Spi_IT() = default;

        explicit Spi_IT(SPI_HandleTypeDef& spiHandle_);
        void OnRxComplete();
        void OnTxComplete();
        void NotifyDataIsRead();
    };
}

#endif

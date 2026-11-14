#include "Spi_IT.hpp"

#if defined(HAL_SPI_MODULE_ENABLED)

namespace Peripherals::HAL
{
    Spi_IT::Spi_IT(SPI_HandleTypeDef& spiHandle_)
        : spiHandle(spiHandle_)
    {}

    SpiResult Spi_IT::Transmit(std::span<uint8_t> data)
    {
        if (state != SpiState::Idle)
            return SpiResult::Busy;

        state = SpiState::TxBusy;
        const bool success = HAL_SPI_Transmit_IT( &spiHandle
                                                , data.data()
                                                , data.size()) == HAL_OK;
        
        if (!success)
            state = SpiState::Error;
        return success ? SpiResult::Success : SpiResult::Error;
    }

    SpiResult Spi_IT::Receive(std::span<uint8_t> buffer)
    {
        if (state != SpiState::Idle)
            return SpiResult::Busy;

        state = SpiState::RxBusy;
        const bool success = HAL_SPI_Receive_IT( &spiHandle
                                                , buffer.data()
                                                , buffer.size()) == HAL_OK;
        
        if (!success)
            state = SpiState::Error;
        return success ? SpiResult::Success : SpiResult::Error;
    }

    void Spi_IT::OnRxComplete()
    {
        if (state == SpiState::RxBusy)
            state = SpiState::RxDone;
    }

    void Spi_IT::OnTxComplete()
    {
        if (state == SpiState::TxBusy)
            state = SpiState::Idle;
    }

    void Spi_IT::NotifyDataIsRead()
    {
        if (state == SpiState::RxDone)
            state = SpiState::Idle;
    }    
}

#endif

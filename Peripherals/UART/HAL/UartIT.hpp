#pragma once

#include "IUart.hpp"
#include "Utils/RingBuffer.hpp"

extern "C"
{
#include "stm32l4xx_hal.h"
}

namespace Peripherals
{
    namespace HAL
    {
        template<std::size_t BufferSize = 64>
        class UartIT : public IUart<UartIT<BufferSize>>
        {
        friend IUart<UartIT<BufferSize>>;
        private:
            UART_HandleTypeDef& huart;
            std::size_t overflowCount{ 0 };
            RingBuffer<BufferSize> rxBuffer{};
            uint8_t rxByte{};

            void Poll_Impl(){}

            std::optional<uint8_t> Read_Impl()
            {
                return rxBuffer.Pop();
            }

            void Transmit_Impl(const uint8_t* data, size_t size)
            {
                HAL_UART_Transmit(&huart, const_cast<uint8_t*>(data), size, HAL_MAX_DELAY);
            }

        public:
            UartIT() = delete;
            UartIT(const UartIT&) = delete;
            UartIT(UartIT&&) = delete;
            UartIT& operator=(const UartIT&) = delete;
            UartIT& operator=(UartIT&&) = delete;
            ~UartIT() = default;
            explicit UartIT(UART_HandleTypeDef& huart_)
                : huart(huart_)
            {};

            void StartReceiveIT()
            {
                HAL_UART_Receive_IT(&huart, &rxByte, 1);
            }

            void RxCpltCallback()
            {
                if (!rxBuffer.Push(rxByte))
                    ++overflowCount;
    
                StartReceiveIT();
            }

            // TO DO:
            // - Interrupt version
            // - Line parser - done
        };
    }
}
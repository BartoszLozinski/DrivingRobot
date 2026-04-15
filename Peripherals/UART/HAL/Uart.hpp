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
        class Uart : public IUart<Uart<BufferSize>>
        {
        friend IUart<Uart<BufferSize>>;
        private:
            UART_HandleTypeDef& huart;
            std::size_t overflowCount{ 0 };
            RingBuffer<BufferSize> rxBuffer{};

            std::optional<uint8_t> Read_Impl()
            {
                return rxBuffer.Pop();
            }

            void Transmit_Impl(const uint8_t* data, size_t size)
            {
                HAL_UART_Transmit(&huart, const_cast<uint8_t*>(data), size, HAL_MAX_DELAY);
            }

        public:
            Uart() = delete;
            Uart(const Uart&) = delete;
            Uart(Uart&&) = delete;
            Uart& operator=(const Uart&) = delete;
            Uart& operator=(Uart&&) = delete;
            ~Uart() = default;
            explicit Uart(UART_HandleTypeDef& huart_)
                : huart(huart_)
            {};

            void Poll()
            {
                uint8_t byte;

                while (HAL_UART_Receive(&huart, &byte, 1, 0) == HAL_OK)
                {
                    if (!this->rxBuffer.Push(byte))
                        ++overflowCount;
                }
            }
        };
    }
}
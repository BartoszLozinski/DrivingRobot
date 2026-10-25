#pragma once

#include "IUart.hpp"

namespace Peripherals
{
    class IUart_IT : public IUart
    {
    public:
        IUart_IT() = default;
        IUart_IT(const IUart_IT&) = delete;
        IUart_IT(IUart_IT&&) = delete;
        IUart_IT& operator=(const IUart_IT&) = delete;
        IUart_IT& operator=(IUart_IT&&) = delete;
        virtual ~IUart_IT() = default;
        
        virtual void ProcessTx() = 0;
        virtual void IRQHandler() = 0;
    };
};
#pragma once

#include <array>
#include <optional>

namespace Peripherals
{
    template<typename Implementation>
    class IUart
    {
    public:
        IUart() = default;
        IUart(const IUart&) = delete;
        IUart(IUart&&) = delete;
        IUart& operator=(const IUart&) = delete;
        IUart& operator=(IUart&&) = delete;
        ~IUart() = default;
        
        std::optional<uint8_t> Read()
        {
            return static_cast<Implementation*>(this)->Read_Impl();
        }

        void Transmit(const uint8_t* data, size_t size)
        {
            static_cast<Implementation*>(this)->Transmit_Impl(data, size);
        };


    };
};
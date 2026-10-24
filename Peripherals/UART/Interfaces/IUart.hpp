#pragma once

#include <array>
#include <optional>
#include <cstdint>

namespace Peripherals
{
    class IUart
    {
    public:
        IUart() = default;
        IUart(const IUart&) = delete;
        IUart(IUart&&) = delete;
        IUart& operator=(const IUart&) = delete;
        IUart& operator=(IUart&&) = delete;
        virtual ~IUart() = default;
        
        [[nodiscard]] virtual std::optional<uint8_t> Read() = 0;

        virtual void Transmit(const uint8_t* data, size_t size) = 0;
    };
};
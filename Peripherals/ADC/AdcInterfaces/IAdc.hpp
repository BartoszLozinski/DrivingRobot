#pragma once
#include <cstdint>

namespace Peripherals
{
    class IAdc
    {
    public:
        virtual ~IAdc() = default;

        virtual void Start() = 0;
        virtual void Calibrate() = 0;
        virtual uint32_t Read() const = 0; 
        virtual float ReadVoltage() const = 0;
    };
}
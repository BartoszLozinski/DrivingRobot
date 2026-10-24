#pragma once

namespace Peripherals
{
    enum class GpioState
    {
        Low = 0,
        High = 1,
    };

    class IGpioOutput
    {
    public:
        virtual void Set() = 0;
        virtual void Clear() = 0;
        virtual void Toggle() = 0;
        virtual GpioState GetState() const = 0;

        virtual ~IGpioOutput() = default;
    };
}
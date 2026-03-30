#pragma once

namespace Peripherals
{
    enum class GpioOutputState
    {
        Set,
        Reset,
    };

    template<typename Implementation>
    class IGpioOutput
    {
    public:
        void Set() { static_cast<Implementation*>(this)->Set_Impl(); }
        void Clear() { static_cast<Implementation*>(this)->Clear_Impl(); }
        void Toggle() { static_cast<Implementation*>(this)->Toggle_Impl(); }
        GpioOutputState GetState() const { return static_cast<const Implementation*>(this)->GetState_Impl(); }
    };
}
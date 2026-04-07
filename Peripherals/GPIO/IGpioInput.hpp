#pragma once

namespace Peripherals
{
    template<typename Implementation>
    class IGpioInput
    {
    public:
        uint32_t Read() const { return static_cast<const Implementation*>(this)->Read_Impl(); }
    };

    /*
    template<typename Implementation>
    class IGpioInputIT : public IGpioInput<Implementation>
    {
    public:
        // TO DO
        // interrupts
    };
    */
}
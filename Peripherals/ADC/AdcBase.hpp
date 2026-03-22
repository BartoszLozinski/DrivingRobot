#pragma once

namespace Peripherals
{
    template<typename Implementation>
    class AdcBase
    {
    protected:
        void Start() { static_cast<Implementation*>(this)->Start_Impl(); };
        void Calibrate() {static_cast<Implementation*>(this)->Calibrate_Impl(); }

    public:

        uint32_t Read() const { return static_cast<const Implementation*>(this)->Read_Impl(); } 
        float ReadVoltage() const { return Read() * Implementation::voltageRef / Implementation::maxValue; }
    };
}
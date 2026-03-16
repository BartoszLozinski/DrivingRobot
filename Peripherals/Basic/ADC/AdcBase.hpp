#pragma once

namespace Peripherals
{
    template<typename Implementation>
    class AdcBase
    {
    protected:

        void Start() { static_cast<Implementation*>(this)->Start_Impl(); };
        void Calibrate() {static_cast<Implementation*>(this)->Calibrate_Impl(); }

        AdcBase() = default;

    public:

        uint32_t Read() { return static_cast<Implementation*>(this)->Read_Impl(); } 
    };
}
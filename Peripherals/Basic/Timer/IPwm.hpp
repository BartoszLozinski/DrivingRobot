#pragma once

#include <cstdint>

namespace Peripherals
{
    template<typename Implementation>
    class IPwm
    {
    protected:
        IPwm() = default;
    public:
        void Start() { static_cast<Implementation*>(this)->Start_Impl(); };
        void Stop() { static_cast<Implementation*>(this)->Stop_Impl(); };
        void SetPulse(const uint32_t pulse) { static_cast<Implementation*>(this)->SetPulse_Impl(pulse); };
        ~IPwm() = default;
    };
};
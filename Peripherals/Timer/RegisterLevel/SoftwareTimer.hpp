#pragma once

#include "Interfaces/TimerBase.hpp"
#include <cstdint>

namespace RegisterLevel
{
    class SoftwareTimer : public TimerBase<SoftwareTimer>
    {
    friend TimerBase<SoftwareTimer>;
    private:
        [[nodiscard]] uint32_t Now_Impl() const;
    public:
        SoftwareTimer(const uint32_t delay_);
    };
}
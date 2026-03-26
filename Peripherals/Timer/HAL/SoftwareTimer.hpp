#pragma once

#include "../TimerBase.hpp"
#include <cstdint>

namespace HAL
{
    class SoftwareTimer : public TimerBase<SoftwareTimer>
    {
    friend TimerBase<SoftwareTimer>;
    private:
        [[nodiscard]] uint32_t Now_Impl() const;

    public:
        SoftwareTimer() = delete;
        SoftwareTimer(const SoftwareTimer&) = delete;
        SoftwareTimer(SoftwareTimer&&) = delete;
        SoftwareTimer& operator=(const SoftwareTimer&) = delete;
        SoftwareTimer& operator=(SoftwareTimer&&) = delete;
        SoftwareTimer(const uint32_t delay_);
    };
};
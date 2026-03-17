#pragma once

#include "TimerBase.hpp"
#include <cstdint>

class SoftwareTimerHAL : public TimerBase<SoftwareTimerHAL>
{
friend TimerBase<SoftwareTimerHAL>;
private:
    [[nodiscard]] uint32_t Now_Impl() const;

public:
    SoftwareTimerHAL() = delete;
    SoftwareTimerHAL(const SoftwareTimerHAL&) = delete;
    SoftwareTimerHAL(SoftwareTimerHAL&&) = delete;
    SoftwareTimerHAL& operator=(const SoftwareTimerHAL&) = delete;
    SoftwareTimerHAL& operator=(SoftwareTimerHAL&&) = delete;
    SoftwareTimerHAL(const uint32_t delay_);
};

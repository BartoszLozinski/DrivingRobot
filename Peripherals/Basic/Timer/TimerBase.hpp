#pragma once

#include <cstdint>

template<typename Implementation>
class TimerBase
{
protected:
    uint32_t delay = 0;
    uint32_t lastEnabled = 0;
    explicit TimerBase(const uint32_t delay_)
        : delay(delay_)
        , lastEnabled(Now())
    {};

public:
    TimerBase() = delete;
    TimerBase(const TimerBase&) = delete;
    TimerBase(TimerBase&&) = delete;
    TimerBase& operator=(const TimerBase&) = delete;
    TimerBase& operator=(TimerBase&&) = delete;

    [[nodiscard]] bool IsExpired() const
    {
        if ((Now() - lastEnabled) >= delay)
            return true;

        return false;
    }

    [[nodiscard]] uint32_t Now() const { return static_cast<const Implementation*>(this)->Now_Impl(); }
    void Reset() { lastEnabled = Now(); }
    [[nodiscard]] uint32_t TimeExpired() const { return Now() - lastEnabled; }
};

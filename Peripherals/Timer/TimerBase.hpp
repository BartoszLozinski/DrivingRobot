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

    [[nodiscard]] bool IsExpired() const
    {
        if ((Now() - lastEnabled) >= delay)
            return true;

        return false;
    }

    [[nodiscard]] uint32_t Now() const { return static_cast<const Implementation*>(this)->Now_Impl(); }
    void Reset() { lastEnabled = Now(); }
    [[nodiscard]] uint32_t TimeElapsed() const { return Now() - lastEnabled; }
};

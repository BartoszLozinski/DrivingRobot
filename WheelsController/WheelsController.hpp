#pragma once

#include "Peripherals/Timer/TimerInterfaces/IPwm.hpp"

// TODO - add unit tests
// TODO - add class that parses uart commands
// TODO - Forward/Backward first - with smooth intensity controlling

enum class Throttle
{
    BACKWARD,
    STOP,
    FORWARD,
};

// Wrote for LEDs mocks right now. When physical motors controller will be available, then it will be updated

class WheelsController
{
private:
    Peripherals::IPwm& left;
    Peripherals::IPwm& right;

    static constexpr uint32_t IDLE_PULSE_PERCENT = 0;
    static constexpr uint32_t PULSE_INCREASEMENT_PERCENT = 2;

    void ThrottleControl(const Throttle throttleCommand);
    void Backward(const uint32_t currentPulseLeftPercent, const uint32_t currentPulseRightPercent);
    void Forward(const uint32_t currentPulseLeftPercent, const uint32_t currentPulseRightPercent);
    void Idle(const uint32_t currentPulseLeftPercent, const uint32_t currentPulseRightPercent);

public:
    WheelsController(Peripherals::IPwm& left_, Peripherals::IPwm& right_);
    Throttle ParseControlOption(const uint8_t controlChar) const;
    void Control(const Throttle throttleCommand);

};

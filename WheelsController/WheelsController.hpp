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

    void Accelerate(const Throttle throttleCommand);

public:
    WheelsController(Peripherals::IPwm& left_, Peripherals::IPwm& right_);


};

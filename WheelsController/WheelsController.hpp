#pragma once

#include "Peripherals/Timer/TimerInterfaces/IPwm.hpp"

class WheelsController
{
private:
    Peripherals::IPwm& left;
    Peripherals::IPwm& right;

public:
    WheelsController(Peripherals::IPwm& left_, Peripherals::IPwm& right_);


};

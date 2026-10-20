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

enum class Steering
{
    LEFT,
    RIGHT,
    STRAIGHT,
};

// Wrote for LEDs mocks right now. When physical motors controller will be available, then it will be updated

class WheelsController
{
private:
    Peripherals::IPwm& left;
    Peripherals::IPwm& right;

    static constexpr uint32_t IDLE_PULSE_PERCENT = 0;
    static constexpr uint32_t PULSE_INCREASEMENT_PERCENT = 4;
    static constexpr uint32_t STEERING_PULSE_DIFFERENCE_PERCENT = 10;
    static constexpr uint8_t MAX_FORWARD_PERCENT_PULSE = 60;

    void ThrottleControl(const Throttle throttleCommand);
    void SteeringControl(const Steering steeringCommand);
    void Backward(const uint32_t currentPulseLeftPercent, const uint32_t currentPulseRightPercent);
    void Forward(const uint32_t currentPulseLeftPercent, const uint32_t currentPulseRightPercent);
    void Idle(const uint32_t currentPulseLeftPercent, const uint32_t currentPulseRightPercent);
    void Left(const uint32_t currentPulseRightPercent);
    void Right(const uint32_t currentPulseLeftPercent);
    void Straight(const uint32_t currentPulseLeftPercent, const uint32_t currentPulseRightPercent);


public:
    WheelsController(Peripherals::IPwm& left_, Peripherals::IPwm& right_);
    Throttle ParseThrottleControlOption(const uint8_t controlChar) const;
    Steering ParseSteeringControlOption(const uint8_t controlChar) const;

    void Control(const Throttle throttleCommand, const Steering steeringCommand);

};

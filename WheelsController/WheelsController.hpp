#pragma once

#include "Peripherals/Timer/Interfaces/IPwm.hpp"

// TODO - add unit tests
// TODO - add class that parses uart commands
// TODO - Forward/Backward first - with smooth intensity controlling

enum class Throttle
{
    IDLE = 0,
    FORWARD = 1,
    BACKWARD = 2,
};

enum class Steering
{
    STRAIGHT = 0,
    LEFT = 1,
    RIGHT = 2,
};

// Wrote for LEDs mocks right now. When physical motors controller will be available, then it will be updated

class WheelsController
{
private:
    static constexpr uint8_t IDLE_PULSE_PERCENT = 0;
    static constexpr uint8_t PULSE_INCREASEMENT_PERCENT = 4;
    static constexpr uint8_t STEERING_PULSE_DIFFERENCE_PERCENT = 20;
    static constexpr uint8_t MAX_STRAIGHT_PERCENT_PULSE = 40;

    void ThrottleControl();
    void SteeringControl();
    void Idle(const uint32_t currentPulseLeftPercent, const uint32_t currentPulseRightPercent);

    [[nodiscard]] uint8_t GetSaturatedPulseIncreasement(const uint8_t currentPulsePercent, const uint8_t maxPulse);
    [[nodiscard]] uint8_t GetSaturatedPulseDecreasement(const uint8_t currentPulsePercent);

protected:
    Peripherals::IPwm& left;
    Peripherals::IPwm& right;

    Throttle throttleCommand = Throttle::IDLE;
    Steering steeringCommand = Steering::STRAIGHT;

public:
    WheelsController(Peripherals::IPwm& left_, Peripherals::IPwm& right_);
    void Control();
    void ParseThrottleControlOption(const uint8_t controlChar);
    void ParseSteeringControlOption(const uint8_t controlChar);
};

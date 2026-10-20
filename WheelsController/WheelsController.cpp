#include "WheelsController.hpp"

WheelsController::WheelsController(Peripherals::IPwm& left_, Peripherals::IPwm& right_)
    : left(left_)
    , right(right_)
{};

void WheelsController::Backward(const uint32_t currentPulseLeftPercent, const uint32_t currentPulseRightPercent)
{
    const uint32_t calculatedPulseLeft = (currentPulseLeftPercent > PULSE_INCREASEMENT_PERCENT)
                                       ? (currentPulseLeftPercent - PULSE_INCREASEMENT_PERCENT)
                                       : IDLE_PULSE_PERCENT;

    left.SetPulsePercent(calculatedPulseLeft);

    const uint32_t calculatedPulseRight = (currentPulseRightPercent > PULSE_INCREASEMENT_PERCENT)
                                        ? (currentPulseRightPercent - PULSE_INCREASEMENT_PERCENT)
                                        : IDLE_PULSE_PERCENT;

    right.SetPulsePercent(calculatedPulseRight);
}

void WheelsController::Control(const Throttle throttleCommand, const Steering steeringCommand)
{
    ThrottleControl(throttleCommand);
    SteeringControl(steeringCommand);
}

void WheelsController::Forward(const uint32_t currentPulseLeftPercent, const uint32_t currentPulseRightPercent)
{
    // saturation inside SetPulse method
    const uint32_t calculatedPulseLeft = currentPulseLeftPercent + PULSE_INCREASEMENT_PERCENT;
    left.SetPulsePercent(calculatedPulseLeft);
    const uint32_t calculatedPulseRight = currentPulseRightPercent + PULSE_INCREASEMENT_PERCENT;
    right.SetPulsePercent(calculatedPulseRight);
}

void WheelsController::Idle(const uint32_t currentPulseLeftPercent, const uint32_t currentPulseRightPercent)
{
    // Controlling function - right now, same as Backward, as there is no reversion yet
    static constexpr uint32_t PULSE_DECAY_PERCENT = 5; 
    const uint32_t calculatedPulseLeft = (currentPulseLeftPercent > PULSE_DECAY_PERCENT) 
                                       ? (currentPulseLeftPercent - PULSE_DECAY_PERCENT)
                                       : IDLE_PULSE_PERCENT;

    left.SetPulsePercent(calculatedPulseLeft);

    const uint32_t calculatedPulseRight = (currentPulseRightPercent > PULSE_DECAY_PERCENT) 
                                        ? (currentPulseRightPercent - PULSE_DECAY_PERCENT)
                                        : IDLE_PULSE_PERCENT;

    right.SetPulsePercent(calculatedPulseRight);
}

void WheelsController::Left(const uint32_t currentPulseRightPercent)
{
    right.SetPulsePercent(currentPulseRightPercent + STEERING_PULSE_DIFFERENCE_PERCENT);
}

void WheelsController::Right(const uint32_t currentPulseLeftPercent)
{
    left.SetPulsePercent(currentPulseLeftPercent + STEERING_PULSE_DIFFERENCE_PERCENT);
}

void WheelsController::Straight(const uint32_t currentPulseLeftPercent, const uint32_t currentPulseRightPercent)
{
    const auto pulseOfBoth = currentPulseLeftPercent > currentPulseRightPercent ? currentPulseRightPercent : currentPulseLeftPercent;
    left.SetPulsePercent(pulseOfBoth);
    right.SetPulsePercent(pulseOfBoth);
}

Throttle WheelsController::ParseThrottleControlOption(const uint8_t controlChar) const
{
    //for now only steering forward
    switch(controlChar)
    {
    case 'w':
    case 'L':
    case 'R':
        return Throttle::FORWARD;
    case 's':
        return Throttle::BACKWARD;
    default:
        return Throttle::STOP;
    }
}

Steering WheelsController::ParseSteeringControlOption(const uint8_t controlChar) const
{
    switch(controlChar)
    {
    case 'l':
    case 'L':
        return Steering::LEFT;
    case 'r':
    case 'R':
        return Steering::RIGHT;
    default:
        return Steering::STRAIGHT;
    }
}


void WheelsController::ThrottleControl(const Throttle throttleCommand)
{
    const auto maxPulseLeft = left.GetMaxCounter();
    const auto maxPulseRight = right.GetMaxCounter();
    volatile const auto currentPulseLeftPercent = left.GetPulse() * 100 / maxPulseLeft;
    volatile const auto currentPulseRightPercent = right.GetPulse() * 100 / maxPulseRight;

    switch(throttleCommand)
    {
    using enum Throttle;
    case BACKWARD:
        //No reversion yet
        Backward(currentPulseLeftPercent, currentPulseRightPercent);
        break;
    case STOP:
        Idle(currentPulseLeftPercent, currentPulseRightPercent);
        break;
    case FORWARD:
        Forward(currentPulseLeftPercent, currentPulseRightPercent);
        break;
    default:
        break;
    };
}

void WheelsController::SteeringControl(const Steering steeringCommand)
{
    const auto maxPulseLeft = left.GetMaxCounter();
    const auto maxPulseRight = right.GetMaxCounter();
    volatile const auto currentPulseLeftPercent = left.GetPulse() * 100 / maxPulseLeft;
    volatile const auto currentPulseRightPercent = right.GetPulse() * 100 / maxPulseRight;

    switch(steeringCommand)
    {
    using enum Steering;
    case LEFT:
        //No reversion yet
        Left(currentPulseRightPercent);
        break;
    case RIGHT:
        Right(currentPulseLeftPercent);
        break;
    case STRAIGHT:
        Straight(currentPulseLeftPercent, currentPulseRightPercent);
        break;
    default:
        break;
    };
}

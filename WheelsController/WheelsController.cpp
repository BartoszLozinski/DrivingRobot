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

void WheelsController::Control(const Throttle throttleCommand)
{
    //TODO add steering control, right now only accelerate
    ThrottleControl(throttleCommand);
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

Throttle WheelsController::ParseControlOption(const uint8_t controlChar) const
{
    switch(controlChar)
    {
    case 'w':
        return Throttle::FORWARD;
    case 's':
        return Throttle::BACKWARD;
    default:
        return Throttle::STOP;
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

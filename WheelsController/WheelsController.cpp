#include "WheelsController.hpp"

WheelsController::WheelsController(Peripherals::IPwm& left_, Peripherals::IPwm& right_)
    : left(left_)
    , right(right_)
{};

void WheelsController::Backward(const uint32_t currentPulseLeft, const uint32_t currentPulseRight)
{
    const uint32_t calculatedPulseLeft = (currentPulseLeft > PULSE_INCREASEMENT) ? (currentPulseLeft - PULSE_INCREASEMENT) : IDLE_PULSE;
    left.SetPulse(calculatedPulseLeft);
    const uint32_t calculatedPulseRight = (currentPulseRight > PULSE_INCREASEMENT) ? (currentPulseRight - PULSE_INCREASEMENT) : IDLE_PULSE;
    right.SetPulse(calculatedPulseRight);
}

void WheelsController::Forward(const uint32_t currentPulseLeft, const uint32_t currentPulseRight)
{
    // saturation inside SetPulse method
    const uint32_t calculatedPulseLeft = currentPulseLeft + PULSE_INCREASEMENT;
    left.SetPulse(calculatedPulseLeft);
    const uint32_t calculatedPulseRight = currentPulseRight + PULSE_INCREASEMENT;
    right.SetPulse(calculatedPulseRight);
}

void WheelsController::Idle(const uint32_t currentPulseLeft, const uint32_t currentPulseRight)
{
    // Controlling function - right now, same as Backward, as there is no reversion yet
    static constexpr uint32_t PULSE_DECAY = 25; 
    const uint32_t calculatedPulseLeft = (currentPulseLeft > PULSE_DECAY) ? (currentPulseLeft - PULSE_INCREASEMENT) : IDLE_PULSE;
    left.SetPulse(calculatedPulseLeft);
    const uint32_t calculatedPulseRight = (currentPulseRight > PULSE_DECAY) ? (currentPulseRight - PULSE_INCREASEMENT) : IDLE_PULSE;
    right.SetPulse(calculatedPulseRight);
}


void WheelsController::ThrottleControl(const Throttle throttleCommand)
{
    // TODO update to operate on percentage, not pulse values

    volatile const auto currentPulseLeft = left.GetPulse();
    volatile const auto currentPulseRight = right.GetPulse();
    [[maybe_unused]] volatile const auto maxPulse = left.GetMaxCounter();

    switch(throttleCommand)
    {
    using enum Throttle;
    case BACKWARD:
        //No reversion yet
        Backward(currentPulseLeft, currentPulseRight);
        break;
    case STOP:
        Idle(currentPulseLeft, currentPulseRight);
        break;
    case FORWARD:
        Forward(currentPulseLeft, currentPulseRight);
        break;
    default:
        break;
    };
}

void WheelsController::Control(const Throttle throttleCommand)
{
    //TODO add steering control, right now only accelerate
    ThrottleControl(throttleCommand);
}


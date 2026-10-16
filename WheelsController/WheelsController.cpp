#include "WheelsController.hpp"

WheelsController::WheelsController(Peripherals::IPwm& left_, Peripherals::IPwm& right_)
    : left(left_)
    , right(right_)
{};

void WheelsController::Accelerate(const Throttle throttleCommand)
{
    // TODO update to operate on percentage, not pulse values
    static constexpr uint32_t IDLE_PULSE = 0;
    static constexpr uint32_t PULSE_INCREASEMENT = 100;
    volatile const auto currentPulseLeft = left.GetPulse();
    volatile const auto currentPulseRight = right.GetPulse();
    int32_t calculatedPulseLeft = 0;
    int32_t calculatedPulseRight = 0;

    switch(throttleCommand)
    {
    using enum Throttle;
    case BACKWARD:
        //No reversion yet
        calculatedPulseLeft = (currentPulseLeft > PULSE_INCREASEMENT) ? (currentPulseLeft - PULSE_INCREASEMENT) : IDLE_PULSE;
        left.SetPulse(calculatedPulseLeft);
        calculatedPulseRight = (currentPulseRight > PULSE_INCREASEMENT) ? (currentPulseRight - PULSE_INCREASEMENT) : IDLE_PULSE;
        right.SetPulse(calculatedPulseRight);
        break;
    case STOP:
        // Controlling function - right now, same as Backward, as there is no reversion yet
        static constexpr uint32_t PULSE_DECAY = 25; 
        calculatedPulseLeft = (currentPulseLeft > PULSE_DECAY) ? (currentPulseLeft - PULSE_INCREASEMENT) : IDLE_PULSE;
        left.SetPulse(calculatedPulseLeft);
        calculatedPulseRight = (currentPulseRight > PULSE_DECAY) ? (currentPulseRight - PULSE_INCREASEMENT) : IDLE_PULSE;
        right.SetPulse(calculatedPulseRight);
        break;
    case FORWARD:
        // saturation inside SetPulse method
        calculatedPulseLeft = currentPulseLeft + PULSE_INCREASEMENT;
        left.SetPulse(calculatedPulseLeft);
        calculatedPulseRight = currentPulseRight + PULSE_INCREASEMENT;
        right.SetPulse(calculatedPulseRight);
        break;
    default:
        break;
    };
}

#include "WheelsController.hpp"

WheelsController::WheelsController(Peripherals::IPwm& left_, Peripherals::IPwm& right_)
    : left(left_)
    , right(right_)
{};

void WheelsController::Control()
{
    const auto maxPulseLeft = left.GetMaxCounter();
    const auto maxPulseRight = right.GetMaxCounter();
    volatile const auto currentPulseLeftPercent = left.GetPulse() * 100 / maxPulseLeft;
    volatile const auto currentPulseRightPercent = right.GetPulse() * 100 / maxPulseRight;

    const uint8_t steeringCommandMaxPulseSwitch = (steeringCommand == Steering::STRAIGHT) ? 0 : 1;
    const uint8_t throttleCommandMaxPulseSwitch = (throttleCommand == Throttle::IDLE) ? 0 : 1;

    // todo steering backward left/right
    switch(steeringCommand)
    {
    using enum Steering;
    case LEFT:
        left.SetPulsePercent(GetSaturatedPulseIncreasement(currentPulseLeftPercent, throttleCommandMaxPulseSwitch * MAX_STRAIGHT_PERCENT_PULSE));
        right.SetPulsePercent(GetSaturatedPulseIncreasement(currentPulseRightPercent, throttleCommandMaxPulseSwitch * MAX_STRAIGHT_PERCENT_PULSE +
                                                                                      steeringCommandMaxPulseSwitch * STEERING_PULSE_DIFFERENCE_PERCENT));
 
        break;
    case RIGHT:
        left.SetPulsePercent(GetSaturatedPulseIncreasement(currentPulseLeftPercent, throttleCommandMaxPulseSwitch * MAX_STRAIGHT_PERCENT_PULSE +
                                                                                    steeringCommandMaxPulseSwitch * STEERING_PULSE_DIFFERENCE_PERCENT));
        right.SetPulsePercent(GetSaturatedPulseIncreasement(currentPulseRightPercent, throttleCommandMaxPulseSwitch * MAX_STRAIGHT_PERCENT_PULSE));

        break;
    case STRAIGHT:
        if (throttleCommand == Throttle::FORWARD)
        {
            left.SetPulsePercent(GetSaturatedPulseIncreasement(currentPulseLeftPercent, throttleCommandMaxPulseSwitch * MAX_STRAIGHT_PERCENT_PULSE));
            right.SetPulsePercent(GetSaturatedPulseIncreasement(currentPulseRightPercent, throttleCommandMaxPulseSwitch * MAX_STRAIGHT_PERCENT_PULSE));
        }
        else if (throttleCommand == Throttle::BACKWARD)
        {
            left.SetPulsePercent(GetSaturatedPulseDecreasement(currentPulseLeftPercent));
            right.SetPulsePercent(GetSaturatedPulseDecreasement(currentPulseRightPercent));
        }
        else
        {
            Idle(currentPulseLeftPercent, currentPulseRightPercent);
        }
        break;
    default:
        break;
    };
}

uint8_t WheelsController::GetSaturatedPulseDecreasement(const uint8_t currentPulsePercent)
{
    return (currentPulsePercent > PULSE_INCREASEMENT_PERCENT) ?
           (currentPulsePercent - PULSE_INCREASEMENT_PERCENT) :
           IDLE_PULSE_PERCENT;
}

uint8_t WheelsController::GetSaturatedPulseIncreasement(const uint8_t currentPulsePercent, const uint8_t maxPulse)
{
    const uint32_t calculatedPulse = currentPulsePercent + PULSE_INCREASEMENT_PERCENT;
    return (calculatedPulse > maxPulse ? maxPulse : calculatedPulse);
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

void WheelsController::ParseThrottleControlOption(const uint8_t controlChar)
{
    //for now only steering forward
    switch(controlChar)
    {
    case 'w':
    case 'L':
    case 'R':
        throttleCommand = Throttle::FORWARD;
        break;
    case 's':
        throttleCommand = Throttle::BACKWARD;
        break;
    default:
        throttleCommand = Throttle::IDLE;
        break;
    }
}

void WheelsController::ParseSteeringControlOption(const uint8_t controlChar)
{
    switch(controlChar)
    {
    case 'l':
    case 'L':
        steeringCommand = Steering::LEFT;
        break;
    case 'r':
    case 'R':
        steeringCommand = Steering::RIGHT;
        break;
    default:
        steeringCommand = Steering::STRAIGHT;
        break;
    }
}

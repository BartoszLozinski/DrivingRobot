#pragma once

#include "Mocks/IPwmMocks.hpp"
#include "WheelsController/WheelsController.hpp"

class WheelsControllerWrapper : public WheelsController
{
public:
    WheelsControllerWrapper(Peripherals::IPwm& left_, Peripherals::IPwm& right_)
        : WheelsController(left_, right_)
    {};

    void SetControlOption(const char commandChar);
};

class WheelsControllerFixture : public ::testing::Test
{
public:
    static constexpr uint32_t PULSE_INCREASEMENT_PERCENT = 4;
    static constexpr uint32_t PULSE_DECAY_PERCENT = 5; 

    IPwmMock leftPwmMock{};
    IPwmMock rightPwmMock{};

    WheelsControllerWrapper controller{ leftPwmMock, rightPwmMock };
};
#pragma once

#include "Mocks/IPwmMocks.hpp"
#include "WheelsController/WheelsController.hpp"

class WheelsControllerFixture : public ::testing::Test
{
public:
    static constexpr uint32_t PULSE_INCREASEMENT_PERCENT = 2;
    static constexpr uint32_t PULSE_DECAY_PERCENT = 5; 

    IPwmMock leftPwmMock{};
    IPwmMock rightPwmMock{};

    WheelsController controller{ leftPwmMock, rightPwmMock };
};
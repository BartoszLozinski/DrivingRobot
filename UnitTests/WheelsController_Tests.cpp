#include "WheelsController_Tests.hpp"

void WheelsControllerWrapper::SetControlOption(const char commandChar)
{
    ParseThrottleControlOption(commandChar);
    ParseSteeringControlOption(commandChar);
}

TEST_F(WheelsControllerFixture, ThrottleControlForwardTest)
{
    controller.SetControlOption('w');

    EXPECT_CALL(leftPwmMock, GetMaxCounter()).WillRepeatedly([](){ return 1000; });
    EXPECT_CALL(rightPwmMock, GetMaxCounter()).WillRepeatedly([](){ return 1000; });
    EXPECT_CALL(leftPwmMock, GetPulse()).WillRepeatedly([](){ return 100; });
    EXPECT_CALL(rightPwmMock, GetPulse()).WillRepeatedly([](){ return 100; });
    EXPECT_CALL(leftPwmMock, SetPulsePercent(10 + PULSE_INCREASEMENT_PERCENT)).Times(::testing::AtLeast(1));
    EXPECT_CALL(rightPwmMock, SetPulsePercent(10 + PULSE_INCREASEMENT_PERCENT)).Times(::testing::AtLeast(1));

    controller.Control();
};

TEST_F(WheelsControllerFixture, ThrottleControlBackwardTest)
{
    controller.SetControlOption('s');

    EXPECT_CALL(leftPwmMock, GetMaxCounter()).WillRepeatedly([](){ return 1000; });
    EXPECT_CALL(rightPwmMock, GetMaxCounter()).WillRepeatedly([](){ return 1000; });
    EXPECT_CALL(leftPwmMock, GetPulse()).WillRepeatedly([](){ return 100; });
    EXPECT_CALL(rightPwmMock, GetPulse()).WillRepeatedly([](){ return 100; });
    EXPECT_CALL(leftPwmMock, SetPulsePercent(10 - PULSE_INCREASEMENT_PERCENT)).Times(::testing::AtLeast(1));
    EXPECT_CALL(rightPwmMock, SetPulsePercent(10 - PULSE_INCREASEMENT_PERCENT)).Times(::testing::AtLeast(1));

    controller.Control();
};

TEST_F(WheelsControllerFixture, ThrottleControlIdleTest)
{
    controller.SetControlOption('x');

    EXPECT_CALL(leftPwmMock, GetMaxCounter()).WillRepeatedly([](){ return 1000; });
    EXPECT_CALL(rightPwmMock, GetMaxCounter()).WillRepeatedly([](){ return 1000; });
    EXPECT_CALL(leftPwmMock, GetPulse()).WillRepeatedly([](){ return 100; });
    EXPECT_CALL(rightPwmMock, GetPulse()).WillRepeatedly([](){ return 100; });
    EXPECT_CALL(leftPwmMock, SetPulsePercent(10 - PULSE_DECAY_PERCENT)).Times(::testing::AtLeast(1));
    EXPECT_CALL(rightPwmMock, SetPulsePercent(10 - PULSE_DECAY_PERCENT)).Times(::testing::AtLeast(1));

    controller.Control();
};
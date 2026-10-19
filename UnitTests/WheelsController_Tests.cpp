#include "WheelsController_Tests.hpp"

TEST_F(WheelsControllerFixture, ThrottleControlForwardTest)
{
    EXPECT_CALL(leftPwmMock, GetMaxCounter()).WillOnce([](){ return 1000; });
    EXPECT_CALL(rightPwmMock, GetMaxCounter()).WillOnce([](){ return 1000; });
    EXPECT_CALL(leftPwmMock, GetPulse()).WillOnce([](){ return 100; });
    EXPECT_CALL(rightPwmMock, GetPulse()).WillOnce([](){ return 100; });
    EXPECT_CALL(leftPwmMock, SetPulsePercent(10 + PULSE_INCREASEMENT_PERCENT)).Times(1);
    EXPECT_CALL(rightPwmMock, SetPulsePercent(10 + PULSE_INCREASEMENT_PERCENT)).Times(1);

    controller.Control(controller.ParseControlOption('w'));
};

TEST_F(WheelsControllerFixture, ThrottleControlBackwardTest)
{
    EXPECT_CALL(leftPwmMock, GetMaxCounter()).WillOnce([](){ return 1000; });
    EXPECT_CALL(rightPwmMock, GetMaxCounter()).WillOnce([](){ return 1000; });
    EXPECT_CALL(leftPwmMock, GetPulse()).WillOnce([](){ return 100; });
    EXPECT_CALL(rightPwmMock, GetPulse()).WillOnce([](){ return 100; });
    EXPECT_CALL(leftPwmMock, SetPulsePercent(10 - PULSE_INCREASEMENT_PERCENT)).Times(1);
    EXPECT_CALL(rightPwmMock, SetPulsePercent(10 - PULSE_INCREASEMENT_PERCENT)).Times(1);

    controller.Control(controller.ParseControlOption('s'));
};

TEST_F(WheelsControllerFixture, ThrottleControlIdleTest)
{
    EXPECT_CALL(leftPwmMock, GetMaxCounter()).WillOnce([](){ return 1000; });
    EXPECT_CALL(rightPwmMock, GetMaxCounter()).WillOnce([](){ return 1000; });
    EXPECT_CALL(leftPwmMock, GetPulse()).WillOnce([](){ return 100; });
    EXPECT_CALL(rightPwmMock, GetPulse()).WillOnce([](){ return 100; });
    EXPECT_CALL(leftPwmMock, SetPulsePercent(10 - PULSE_DECAY_PERCENT)).Times(1);
    EXPECT_CALL(rightPwmMock, SetPulsePercent(10 - PULSE_DECAY_PERCENT)).Times(1);

    controller.Control(controller.ParseControlOption('x'));
};
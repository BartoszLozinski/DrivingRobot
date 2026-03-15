#!/bin/bash

openocd -f /usr/share/openocd/scripts/interface/stlink.cfg \
        -f /usr/share/openocd/scripts/target/stm32l4x.cfg \
        -c "program build/Core/DrivingRobot verify reset exit"

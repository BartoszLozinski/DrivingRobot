set(target_name Timer_RegisterLevel)

set(${target_name}_hpp
    SoftwareTimer.hpp
)

set(${target_name}_src
    SoftwareTimer.cpp
)

configure_library(${target_name}
    SOURCES ${${target_name}_src}
    HEADERS ${${target_name}_hpp}
)

configure_embedded_target(${target_name})

target_link_directories(${target_name}
    INTERFACE
        TimerInterfaces
)

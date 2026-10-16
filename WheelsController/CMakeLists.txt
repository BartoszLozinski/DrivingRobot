set(target_name WheelsController)

set(${target_name}_hpp
    WheelsController.hpp
)

set(${target_name}_src
    WheelsController.cpp
)

configure_library(${target_name}
    SOURCES ${${target_name}_srcs}
    HEADERS ${${target_name}_hpp}
)

target_link_libraries(${target_name}
    INTERFACE
        TimerInterfaces
)
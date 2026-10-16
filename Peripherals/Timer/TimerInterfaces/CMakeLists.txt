set(target_name TimerInterfaces)

set(${target_name}_hpp
    TimerBase.hpp
    InputCaptureInterface.hpp
    IPwm.hpp
)

# TODO to the same with all other interfaces

configure_library(${target_name}
    HEADER_ONLY True
    HEADERS ${${target_name}_hpp}
)

configure_embedded_target(${target_name}
    HEADER_ONLY True
)

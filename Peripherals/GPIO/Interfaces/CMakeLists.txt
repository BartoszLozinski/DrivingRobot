set(target_name GpioInterfaces)

set(${target_name}_hpp
    IGpioInput.hpp
    IGpioOutput.hpp
)

configure_library(${target_name}
    HEADER_ONLY True
    HEADERS ${${target_name}_hpp}
)

configure_embedded_target(${target_name}
    HEADER_ONLY True
)

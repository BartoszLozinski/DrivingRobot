set(target_name Gpio_RegisterLevel)

set(${target_name}_hpp
    GpioBase.hpp
    GpioEnums.hpp
    GpioOutput.hpp
    GpioInput.hpp
    GpioAnalog.hpp
    GpioAlternate.hpp
)

configure_library(${target_name}
    HEADER_ONLY True
    HEADERS ${${target_name}_hpp}
)

configure_embedded_target(${target_name}
    HEADER_ONLY True
)
configure_HAL_target(${target_name}
    HEADER_ONLY True
)

target_link_libraries(${target_name}
    INTERFACE
        Config
        GpioInterfaces
)
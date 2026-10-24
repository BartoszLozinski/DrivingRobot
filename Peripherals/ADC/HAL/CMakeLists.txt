set(target_name ADC_HAL)

set(${target_name}_src  Adc.cpp
)

set(${target_name}_hpp  Adc.hpp
)

configure_library(${target_name}
    SOURCES ${${target_name}_src}
    HEADERS ${${target_name}_hpp}
)
configure_embedded_target(${target_name})
configure_HAL_target(${target_name})

target_link_libraries(${target_name}
    INTERFACE
        AdcInterfaces)
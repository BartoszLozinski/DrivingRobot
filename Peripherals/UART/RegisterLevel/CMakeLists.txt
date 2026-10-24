set(target_name UART_RegisterLevel)

set(${target_name}_hpp  ../Utils/RingBuffer.hpp
                        UartBase.hpp
                        Uart.hpp
)

configure_library(${target_name}
    HEADER_ONLY True
    HEADERS ${${target_name}_hpp}
)

target_link_libraries(${target_name}
    INTERFACE
        Config
        UartInterfaces
        Gpio_RegisterLevel)

configure_embedded_target(${target_name}
    HEADER_ONLY True
)

configure_HAL_target(${target_name}
    HEADER_ONLY True
)

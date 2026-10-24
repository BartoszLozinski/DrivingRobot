set(target_name UART_HAL)

set(${target_name}_src  UartIT.cpp
)


set(${target_name}_hpp  Uart.hpp
                        UartIT.hpp
)

configure_library(${target_name}
    SOURCES ${${target_name}_src}
    HEADERS ${${target_name}_hpp}
)

configure_embedded_target(${target_name}
)

configure_HAL_target(${target_name}
)

target_link_libraries(${target_name}
    INTERFACE
        RingBuffer
        UartInterfaces)
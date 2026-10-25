set(target_name UartInterfaces)

set(${target_name}_hpp
    IUart.hpp
    IUart_IT.hpp
)

configure_library(${target_name}
    HEADER_ONLY True
    HEADERS ${${target_name}_hpp}
)

configure_embedded_target(${target_name}
    HEADER_ONLY True
)

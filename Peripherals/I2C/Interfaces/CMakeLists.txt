set(target_name I2CInterfaces)

set(${target_name}_hpp
    I2CInterface.hpp
    I2CInterface_IT.hpp
)

configure_library(${target_name}
    HEADER_ONLY True
    HEADERS ${${target_name}_hpp}
)

configure_embedded_target(${target_name}
    HEADER_ONLY True
)

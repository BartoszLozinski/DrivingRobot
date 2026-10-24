set(target_name I2C_HAL)

set(${target_name}_src  I2C.cpp
                        I2C_IT.cpp
)

set(${target_name}_hpp  I2C.hpp
                        I2C_IT.hpp
)

configure_library(${target_name}
    SOURCES ${${target_name}_src}
    HEADERS ${${target_name}_hpp}
)

configure_embedded_target(${target_name})
configure_HAL_target(${target_name})

target_link_libraries(${target_name}
    INTERFACE
        I2CInterfaces
)

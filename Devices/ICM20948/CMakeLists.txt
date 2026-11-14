set(target_name ICM20948)

set(${target_name}_src  ICM20948Base.cpp
                        ICM20948_Async.cpp
                        ICM20948_Async_Spi.cpp)

set(${target_name}_hpp  ICM20948_Async.hpp
                        ICM20948_Async_Spi.hpp
)

configure_library(${target_name}
    SOURCES ${${target_name}_src}
    HEADERS ${${target_name}_hpp}
)

configure_embedded_target(${target_name})

target_link_libraries(${target_name}
    INTERFACE
        I2CInterfaces
        GpioInterfaces
        SpiInterfaces
)

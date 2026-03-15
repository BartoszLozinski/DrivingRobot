include(../cmake/compiler_flags.cmake)

set(INCLUDE_DIRS
    CMSIS/Device/ST/STM32L4xx/Include
    CMSIS/Include
    STM32L4xx_HAL_Driver/Inc
    STM32L4xx_HAL_Driver/Legacy
)

file(GLOB HAL_SOURCES
    STM32L4xx_HAL_Driver/Src/*.c
)

set(GLOBAL_DEFINES      STM32L476xx
                        USE_HAL_DRIVER
)

set(HAL_FLAGS   -Wall -Wextra)

add_library(STM32_HAL   STATIC  ${C_HAL_SRC} ${HAL_SOURCES})

target_compile_options(STM32_HAL        PRIVATE     ${HAL_FLAGS})
target_include_directories(STM32_HAL
    PUBLIC
        ${CMAKE_SOURCE_DIR}/Core/Inc
        ${INCLUDE_DIRS}
)

target_compile_definitions(STM32_HAL    PUBLIC      ${GLOBAL_DEFINES})

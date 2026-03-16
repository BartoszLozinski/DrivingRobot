set(target_name STM32_HAL)

add_library(${target_name}   STATIC)

file(GLOB HAL_SOURCES
    STM32L4xx_HAL_Driver/Src/*.c
)

target_sources(${target_name}
    PRIVATE
        ${HAL_SOURCES})

target_compile_options(${target_name}
    PRIVATE
        -Wall
        -Wextra
        -Wstrict-prototypes
        -ffunction-sections
        -fdata-sections
        -fverbose-asm # additional comments for generated assembler code
        -MMD # create dependecy files
        $<$<CONFIG:DEBUG>: -Og -g>
        $<$<CONFIG:RELEASE>: -O3>
)

target_include_directories(${target_name}
    PRIVATE
        STM32L4xx_HAL_Driver/Legacy
    PUBLIC
        ${CMAKE_SOURCE_DIR}/Core/Inc
        CMSIS/Device/ST/STM32L4xx/Include
        CMSIS/Include
        STM32L4xx_HAL_Driver/Inc
)

set_target_properties(${target_name}
    PROPERTIES
        C_STANDARD 11
        C_STANDARD_REQUIRED ON
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)

target_compile_definitions(${target_name}
    PRIVATE
        STM32L476xx
        USE_HAL_DRIVER
)

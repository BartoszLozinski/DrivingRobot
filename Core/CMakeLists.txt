set(MAP_FILE "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map")
set(LD_SCRIPT ${CMAKE_SOURCE_DIR}/STM32L476RGTX_FLASH.ld)

add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        Src/main.cpp
        Src/stm32l4xx_hal_msp.c
        Src/stm32l4xx_it.c
        Src/syscalls.c
        Src/sysmem.c
        Src/system_stm32l4xx.c
        Startup/startup_stm32l476rgtx.s
)

target_include_directories(${CMAKE_PROJECT_NAME}    PRIVATE     Core/Inc)
target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
        STM32_HAL
        ADC_HAL
        Timer
)

target_compile_definitions(${CMAKE_PROJECT_NAME}
    PUBLIC 
        STM32L476xx
        USE_HAL_DRIVER
)

target_compile_options(${CMAKE_PROJECT_NAME}
    PRIVATE
        -Wall
        -Werror
        -Wextra
        -Wpedantic
        -fverbose-asm # additional comments for generated assembler code
        -MMD # create dependency file
        
        $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>

        $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti> # disable virtual class information used by dynamic_cast and typeid
        $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions> # disable exceptions handling
        
        $<$<CONFIG:DEBUG>: -Og -g>
        $<$<CONFIG:RELEASE>: -O3>
)

set_target_properties(${CMAKE_PROJECT_NAME}
    PROPERTIES
        C_STANDARD 11
        C_STANDARD_REQUIRED ON
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)

target_link_options(${CMAKE_PROJECT_NAME}           PRIVATE
    -T${LD_SCRIPT}
    -Wl,-Map=${MAP_FILE},-cref
    -Wl,--gc-sections
)

include(../cmake/compiler_flags.cmake)

set(MAP_FILE "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map")
set(LD_SCRIPT ${CMAKE_SOURCE_DIR}/STM32L476RGTX_FLASH.ld)

set(INCLUDE_DIRS    Core/Inc
                    .
)

set(C_CUBE_GENERATED    Src/stm32l4xx_hal_msp.c
                        Src/stm32l4xx_it.c
                        Src/syscalls.c
                        Src/sysmem.c
                        Src/system_stm32l4xx.c
)

set(ASM_SRCS            Startup/startup_stm32l476rgtx.s)

set(GLOBAL_DEFINES      STM32L476xx
                        USE_HAL_DRIVER
)

add_executable(${CMAKE_PROJECT_NAME} Src/main.cpp ${C_CUBE_GENERATED} ${ASM_SRCS})

target_include_directories(${CMAKE_PROJECT_NAME}    PRIVATE     ${INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME}         PUBLIC STM32_HAL)
target_compile_definitions(${CMAKE_PROJECT_NAME}    PUBLIC      ${GLOBAL_DEFINES})
set_source_files_properties(${ASM_SRCS}             PROPERTIES  LANGUAGE ASM)

target_link_options(${CMAKE_PROJECT_NAME}           PRIVATE
    -T${LD_SCRIPT}
    -Wl,-Map=${MAP_FILE},-cref
    -Wl,--gc-sections
)

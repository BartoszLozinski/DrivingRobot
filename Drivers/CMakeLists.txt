include(../cmake/compiler_flags.cmake)

set(INCLUDE_DIRS
    CMSIS/Device/ST/STM32L4xx/Include
    CMSIS/Include
    STM32L4xx_HAL_Driver/Inc
    STM32L4xx_HAL_Driver/Legacy
)

set(C_HAL_SRC       # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.c
                    # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc_ex.c
                    # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c
                    # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c
                    # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c
                    # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c
                    # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c
                    # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c
                    # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c
                    # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c
                    # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c
                    # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c
                    # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c
                    # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c
                    # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c
                    # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c
                    # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c
                    # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c
                    # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c
                    # STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c

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

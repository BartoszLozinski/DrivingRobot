set(target_name Timer)

set(${target_name}_src  SoftwareTimerHAL.cpp
)

set(${target_name}_hpp  TimerBase.hpp
                        SoftwareTimerHAL.hpp
)

add_library(${target_name} STATIC ${${target_name}_src})

target_include_directories(${target_name} PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(${target_name}    PRIVATE     STM32_HAL)
target_compile_options(${target_name}   PRIVATE     -Wall
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

target_compile_definitions(${target_name}
    PRIVATE
        STM32L476xx
        USE_HAL_DRIVER
)

set_target_properties(${target_name}   PROPERTIES  CXX_STANDARD 20     CXX_STANDARD_REQUIRED ON)

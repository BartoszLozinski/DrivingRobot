set(target_name HC_SR04)

set(${target_name}_src  )

set(${target_name}_hpp  HC_SR04.hpp)

add_library(${target_name}  STATIC)

cmake_path(GET  CMAKE_CURRENT_SOURCE_DIR    PARENT_PATH     DEVICES_DIR)

target_sources(${target_name}
    PRIVATE
        ${${target_name}_src}
    PUBLIC
        FILE_SET HEADER
        BASE_DIRS ${DEVICES_DIR}
        FILES   ${${target_name}_hpp}
)

target_link_libraries(Timer)
target_compile_options(${target_name}   PRIVATE     -Wall
                                                    -Werror
                                                    -Wextra
                                                    -Wpedantic
                                                    -fverbose-asm # additional comments for generated assembler code
                                                    -MMD # create dependency file

                                                    #$<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>

                                                    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti> # disable virtual class information used by dynamic_cast and typeid
                                                    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions> # disable exceptions handling

                                                    $<$<CONFIG:DEBUG>: -Og -g>
                                                    $<$<CONFIG:RELEASE>: -O3>
)

set_target_properties(${target_name}   PROPERTIES  CXX_STANDARD 20     CXX_STANDARD_REQUIRED ON)

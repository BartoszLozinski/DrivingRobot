cmake_minimum_required(VERSION 3.28)
project(UnitTests)

include(${CMAKE_SOURCE_DIR}/../cmake/configuration.cmake)

find_package(GTest REQUIRED)
set(CMAKE_CXX_COMPILER  clang++-18)
set(CMAKE_CXX_EXTENSIONS    OFF)

set(GMOCK_LIBRARIES     /usr/lib/libgmock.a
                        /usr/lib/libgmock_main.a
)

set(${PROJECT_NAME}_src     mainTests.cpp
                            ICM20948_Tests.cpp
                            LineParser_Tests.cpp
                            LPS25HB_Tests.cpp
                            RingBuffer_Tests.cpp
                            WheelsController_Tests.cpp
)

set(${PROJECT_NAME}_hpp     Mocks/I2CMock.hpp
                            Mocks/IPwmMocks.hpp
                            ICM20948_Tests.hpp
                            LineParser_Tests.hpp
                            LPS25HB_Tests.hpp
                            RingBuffer_Tests.hpp
                            WheelsController_Tests.hpp
)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH parent_dir)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../Devices ${CMAKE_CURRENT_BINARY_DIR}/Devices)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../WheelsController ${CMAKE_CURRENT_BINARY_DIR}/WheelsController)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../Peripherals/Timer/TimerInterfaces ${CMAKE_CURRENT_BINARY_DIR}/TimerInterfaces)


add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PRIVATE
        ${${PROJECT_NAME}_src}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${parent_dir}
        FILES ${${PROJECT_NAME}_hpp}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${GTEST_LIBRARIES}
        ${GMOCK_LIBRARIES}
        Devices
        TimerInterfaces
        WheelsController
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
        -Wall
        -Werror
        -Wpedantic
        -Wextra
        $<$<CONFIG:DEBUG>: -Og -g>
        $<$<CONFIG:RELEASE>: -O3>
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)

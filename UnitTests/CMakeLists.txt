cmake_minimum_required(VERSION 3.28)
project(UnitTests)

find_package(GTest REQUIRED)
set(CMAKE_CXX_COMPILER  clang++-18)
set(CMAKE_CXX_EXTENSIONS    OFF)


add_executable(${PROJECT_NAME}  mainTests.cpp)

target_link_libraries(${PROJECT_NAME}   PRIVATE     ${GTEST_LIBRARIES}                                                    
)

target_compile_options(${PROJECT_NAME}  PRIVATE     -Wall
                                                    -Werror
                                                    -Wpedantic
                                                    -Wextra
                                                    $<$<CONFIG:DEBUG>: -Og -g>
                                                    $<$<CONFIG:RELEASE>: -O3>
)

set_target_properties(${PROJECT_NAME}   PROPERTIES  CXX_STANDARD 20     CXX_STANDARD_REQUIRED ON)

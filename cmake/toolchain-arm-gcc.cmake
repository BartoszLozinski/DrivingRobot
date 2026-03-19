# System generic
set(CMAKE_SYSTEM_NAME   Generic)

# Setup arm porcessor and bleeding edge toolchain
set(CMAKE_SYSTEM_PROCESSOR cortex-m4)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_NM arm-none-eabi-nm)
set(CMAKE_STRIP arm-none-eabi-strip)
set(CMAKE_RANLIB arm-none-eabi-ranlib)

# When trying to link cross compiled test program, error occurs, so setting test compilation to static library
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

set(CORE_FLAGS  -mcpu=cortex-m4
                -mthumb
                -mfloat-abi=hard
                -mfpu=fpv4-sp-d16
                #-ffast-math
                )              

#set(CMAKE_C_FLAGS_INIT  ${CORE_FLAGS})
#set(CMAKE_CXX_FLAGS_INIT  ${CORE_FLAGS})
#set(CMAKE_ASM_FLAGS_INIT  ${CORE_FLAGS})

# Remove default static libraries
set(CMAKE_C_STANDARD_LIBRARIES "")

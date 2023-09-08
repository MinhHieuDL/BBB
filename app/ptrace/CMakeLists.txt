cmake_minimum_required(VERSION 3.0)
project(ptrace)

# Set the cross-compiler and toolchain file
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER /home/mhle/Learning/BBB/toolchain/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc)

# Set the path to your cross-compiled toolchain
set(CMAKE_FIND_ROOT_PATH /home/mhle/Learning/BBB/toolchain/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf)

# Set source files
set(DEBUG_SRC_FILES
    src/debug.c
)

set(TRACE_SRC_FILES
    src/trace.c
)

# Set executable names using variables
set(DEBUG_EXECUTABLE_NAME "debug")
set(TRACE_EXECUTABLE_NAME "trace")

# Add executables
add_executable(${DEBUG_EXECUTABLE_NAME} ${DEBUG_SRC_FILES})
add_executable(${TRACE_EXECUTABLE_NAME} ${TRACE_SRC_FILES})

# Set include directories
target_include_directories(${DEBUG_EXECUTABLE_NAME} PRIVATE
    include
)

target_include_directories(${TRACE_EXECUTABLE_NAME} PRIVATE
    include
)

# Specify any other compiler flags or options as needed
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# Set output directories for executables
set_target_properties(${DEBUG_EXECUTABLE_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

set_target_properties(${TRACE_EXECUTABLE_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
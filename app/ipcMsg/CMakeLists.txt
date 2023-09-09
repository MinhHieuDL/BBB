cmake_minimum_required(VERSION 3.0)
project(ipcMSG)

# Set the cross-compiler and toolchain file
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER /home/mhle/Learning/BBB/toolchain/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc)

# Set the path to your cross-compiled toolchain
set(CMAKE_FIND_ROOT_PATH /home/mhle/Learning/BBB/toolchain/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf)

# Set source files
set(SERVER_SRC_FILES
    src/server.c
)

set(CLIENT_SRC_FILES
    src/client.c
)

# Set executable names using variables
set(SERVER_EXECUTABLE_NAME "server")
set(CLIENT_EXECUTABLE_NAME "client")

# Add executables
add_executable(${SERVER_EXECUTABLE_NAME} ${SERVER_SRC_FILES})
add_executable(${CLIENT_EXECUTABLE_NAME} ${CLIENT_SRC_FILES})

# Set include directories
target_include_directories(${SERVER_EXECUTABLE_NAME} PRIVATE
    include
)

target_include_directories(${CLIENT_EXECUTABLE_NAME} PRIVATE
    include
)

# Specify any other compiler flags or options as needed
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# Set output directories for executables
set_target_properties(${SERVER_EXECUTABLE_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

set_target_properties(${CLIENT_EXECUTABLE_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
cmake_minimum_required(VERSION 3.0)
project(ipcSocket)

# Set the cross-compiler and toolchain file
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER /home/mhle/Learning/BBB/toolchain/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc)

# Set the path to your cross-compiled toolchain
set(CMAKE_FIND_ROOT_PATH /home/mhle/Learning/BBB/toolchain/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf)

# Set source files
set(SRC_FILES
    src/server.c
)

# Add executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Set include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

# Specify any other compiler flags or options as needed
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

cmake_minimum_required(VERSION 3.0)
project(race)

# Find the threads library (portable way to link pthreads)
find_package(Threads REQUIRED)

# Set source files
set(SRC_FILES
    src/race.c
)

# Set executable names using variables
set(EXECUTABLE_NAME ${PROJECT_NAME})

# Add executables
add_executable(${EXECUTABLE_NAME} ${SRC_FILES})

# Set include directories
target_include_directories(${EXECUTABLE_NAME} PRIVATE
    include
)

# Link pthreads properly (portable across platforms/toolchains)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE Threads::Threads)

# Compiler warnings 
target_compile_options(${EXECUTABLE_NAME} PRIVATE
    -Wall
    -Wextra
)

# Set output directories for executables
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

cmake_minimum_required(VERSION 3.0)
project(race)

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

# Specify any other compiler flags or options as needed
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# Set output directories for executables
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

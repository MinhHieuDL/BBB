cmake_minimum_required(VERSION 3.0)
project(SocketClient)

# Set source files
set(SRC_FILES
    src/client.c
)

# Add executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Set include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    ../lib/include
)

# Build user libraries
add_subdirectory(../lib ${CMAKE_BINARY_DIR}/libBuild)

# Link with user libraries
target_link_libraries(${PROJECT_NAME} 
    remoteHaShaHandler
)

# Specify any other compiler flags or options as needed
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

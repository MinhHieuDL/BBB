cmake_minimum_required(VERSION 3.0)
project(DBTool)

# Set source files
set(SRC_FILES
    src/main.c
    src/userDBHandler.c
)

# Add executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Set include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    ../lib/include
)

# Handling user libraries
# Set the option to indicate that using cross-compiled
option(OPENSSL_CROSS_COMPILE "Indicate cross-compiled to user libraries builder" OFF)

# Build user libraries
add_subdirectory(../lib ${CMAKE_BINARY_DIR}/libBuild)

# Link with user libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
    encodeHandler
    fileHandler
)

# Specify any other compiler flags or options as needed
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

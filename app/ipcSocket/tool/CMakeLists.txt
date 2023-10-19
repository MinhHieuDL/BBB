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
    ../lib/fileHandler/include
)

# Link with OpenSSL
find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL OpenSSL::Crypto)

# Link with fileHandlerLib
target_link_libraries(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/../lib/build/libBuild/libfileHandlerLib.so")

# Specify any other compiler flags or options as needed
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

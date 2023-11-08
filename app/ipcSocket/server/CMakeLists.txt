cmake_minimum_required(VERSION 3.0)

# Set cross toolchain
set(CMAKE_TOOLCHAIN_FILE ../Toolchain/beaglebone_black.toolchain.cmake)

# Define project name
project(socketServer)

# Set source files
set(SRC_FILES
    src/server.c
    src/verifyLoginHandler.c
)

# Add executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Set the path to the cross-compiled OpenSSL libraries and headers
set(OPENSSL_ROOT_DIR ${CMAKE_SOURCE_DIR}/../../../../crossBuildLib/openSSL_BBB)

# Set include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    ../lib/include
)

# Link with OpenSSL
# Tell CMake where to find the OpenSSL package
list(APPEND CMAKE_PREFIX_PATH ${OPENSSL_ROOT_DIR})

# Find the OpenSSL package
find_package(OpenSSL REQUIRED)

# Link your application with OpenSSL
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL OpenSSL::Crypto)

# Handling user libraries
# Override the OpenSSL include path for the encodeHandler library
set(OPENSSL_INCLUDE_PATH "${OPENSSL_ROOT_DIR}/include")

# Build user libraries
add_subdirectory(../lib ${CMAKE_BINARY_DIR}/libBuild)

# Link with user libraries
target_link_libraries(${PROJECT_NAME} 
    encodeHandler
    fileHandler
    csvHandler
    remoteHaShaHandler
)

# Specify any other compiler flags or options as needed
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

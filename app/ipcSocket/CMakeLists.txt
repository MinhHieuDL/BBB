cmake_minimum_required(VERSIO 3.0)
project(remote_login)

# Include the subdirectories

# Set toolchain and build server 
set(CMAKE_TOOLCHAIN_FILE /toolchain/beaglebone_black.toolchain.cmake)
add_subdirectory(server)

# Restore native compiler to build client and DBTool
set(CMAKE_TOOLCHAIN_FILE "") 
add_subdirectory(client)
add_subdirectory(tool)
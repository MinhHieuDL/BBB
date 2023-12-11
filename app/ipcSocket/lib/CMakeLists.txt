cmake_minimum_required(VERSION 3.22)
project(User_Lib LANGUAGES CXX)

# Define the list of libraries
set(LIBRARIES
    fileHandler
    encodeHandler
    csvHandler
    remoteHaShaHandler
)

# Iterate over the libraries and create them
foreach(LIBRARY ${LIBRARIES})
    # Set source files for the current library
    set(SRC_FILES "src/${LIBRARY}.cpp")

    # Add the library and set its properties
    add_library(${LIBRARY} SHARED ${SRC_FILES})
    target_include_directories(${LIBRARY} PUBLIC "include")

    if(${LIBRARY} STREQUAL "encodeHandler")
        if(OPENSSL_CROSS_COMPILE)
            # Set the path to the cross-compiled OpenSSL libraries and headers
            set(OPENSSL_ROOT_DIR ${CMAKE_SOURCE_DIR}/../../../../crossBuildLib/openSSL_BBB)
            # Tell CMake where to find the OpenSSL package
            list(APPEND CMAKE_PREFIX_PATH ${OPENSSL_ROOT_DIR})
            # Set include path for crossbuild openssl
            set(OPENSSL_INCLUDE_PATH "${OPENSSL_ROOT_DIR}/include")
            target_include_directories(${LIBRARY} PUBLIC ${OPENSSL_INCLUDE_PATH})
        endif()
        # Link do OpenSSL
        find_package(OpenSSL REQUIRED)
        target_link_libraries(${LIBRARY} PUBLIC OpenSSL::SSL OpenSSL::Crypto)
    endif()

    set_target_properties(${LIBRARY} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/libBuild")
endforeach()

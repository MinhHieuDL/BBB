cmake_minimum_required(VERSION 3.0)
project(User_Lib)

# Define the list of libraries
set(LIBRARIES
    fileHandler
    encodeHandler
    csvHandler
)

# Iterate over the libraries and create them
foreach(LIBRARY ${LIBRARIES})
    # Set source files for the current library
    set(SRC_FILES "src/${LIBRARY}.c")

    # Add the library and set its properties
    add_library(${LIBRARY} SHARED ${SRC_FILES})
    target_include_directories(${LIBRARY} PUBLIC "include")

    # If the library is encodeHandler, add the OpenSSL include directory
    if(${LIBRARY} STREQUAL "encodeHandler")
        target_include_directories(${LIBRARY} PUBLIC ${OPENSSL_INCLUDE_PATH})
    endif()

    set_target_properties(${LIBRARY} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/libBuild")
endforeach()

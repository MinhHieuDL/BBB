cmake_minimum_required(VERSION 3.0)
project(MyLibraries)

# Set source files for fileHandler library
set(FILE_HANDLER_SRC
    fileHandler/src/fileHandler.c
)

# Set source files for encodeHandler library
set(ENCODE_HANDLER_SRC
    encodeHandler/src/encodeHandler.c
)

# Set source files for csvHandler library
set(CSV_HANDLER_SRC
    csvHandler/src/csvHandler.c
)

# Create libraries
add_library(fileHandlerLib SHARED ${FILE_HANDLER_SRC})
add_library(encodeHandlerLib SHARED ${ENCODE_HANDLER_SRC})
add_library(csvHandler SHARED ${CSV_HANDLER_SRC})

# Set include directories for libraries
target_include_directories(fileHandlerLib PUBLIC fileHandler/include)
target_include_directories(encodeHandlerLib PUBLIC encodeHandler/include)
target_include_directories(csvHandler PUBLIC csvHandler/include)

# Set the common output directory for libraries
set(LIBRARY_OUTPUT_DIR "${CMAKE_BINARY_DIR}/libBuild")
set_target_properties(fileHandlerLib PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_DIR})
set_target_properties(encodeHandlerLib PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_DIR})
set_target_properties(csvHandler PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_DIR})

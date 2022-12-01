cmake_minimum_required(VERSION 3.10)
project(mosaix)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify output binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Gather source files
file(GLOB SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/extern/easyexif/*.cpp
)

# Set the root for include paths
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	extern/easyexif/
	extern/stb/
)

# Build our project
add_executable(${PROJECT_NAME} ${SRC_FILES})

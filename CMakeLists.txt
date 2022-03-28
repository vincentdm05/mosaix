cmake_minimum_required(VERSION 3.10)
project(mosaix)

# Use C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify output binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Gather source files
file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Set the root for include paths
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	extern/stb/
)

# Build our project
add_executable(${PROJECT_NAME} ${SRC_FILES})

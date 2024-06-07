cmake_minimum_required(VERSION 3.10)
project(ESP32Project)

set(CMAKE_CXX_STANDARD 11)

# Add source files here
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

# Create the executable from the source files
add_executable(your_executable ${SOURCES} ${HEADERS})

# Enable testing
enable_testing()
add_test(NAME your_test COMMAND your_executable)

# Enable code coverage
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

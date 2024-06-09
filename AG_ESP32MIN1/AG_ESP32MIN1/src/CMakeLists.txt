cmake_minimum_required(VERSION 3.16)
project(MyESP32Project)

# Define the toolchain file for ESP32 (assuming you have it setup correctly)
set(CMAKE_TOOLCHAIN_FILE $ENV{IDF_PATH}/tools/cmake/toolchain-esp32.cmake)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Add the main source directory
add_subdirectory(src)

# Define the executable
add_executable(${PROJECT_NAME} src/main.cpp)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${IDF_LIBRARIES})

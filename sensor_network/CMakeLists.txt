cmake_minimum_required(VERSION 3.10)
project(SensorNetworkSimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include header files
include_directories(include)

# Glob all .cpp files from src/
file(GLOB SOURCES "src/*.cpp")

# Add executable target
add_executable(sensor_network ${SOURCES})

# Link with threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(sensor_network Threads::Threads)

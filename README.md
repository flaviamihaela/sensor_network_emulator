# Sensor Network Emulator

This project implements a sensor data processing system that simulates the acquisition, processing, and transmission of sensor data from various types of sensors to a central receiver. The system utilizes multithreading, mutexes, condition variables, and design patterns to achieve concurrent and synchronized processing. For more in-depth information check out my wiki for this project: [wiki](https://github.com/flaviamihaela/sensor_network_emulator/wiki)

## Key aspects:
 - Simulates multiple types of sensor objects: temperature, pressure, and capacitive sensors.
 - Utilizes multithreading ( C++11's thread library ) to acquire sensor data concurrently from multiple threads.
 - Implements a central Bus Controller (BC) for coordinating access to shared resources.
 - Uses a Receiver to collect and process sensor data.
 - Implements a Link Access Controller (LAC) to manage access to communication links.
 - Includes error handling for thread synchronization and resource access.
 - Provides efficient handling of sensor data using deque containers.

## Best Features
- 

## Build 
g++ main.cpp globals.cpp sensor.cpp sensor_data.cpp receiver.cpp bus_controller.cpp link.cpp link_access_controller.cpp utils.cpp acquire_sensor_data.cpp transmit_sensor_data.cpp run.cpp -o sensor_network.exe -pthread

## Dependencies
C++11 compiler

## Usage
1. Clone the repository to your local machine.
2. Navigate to the project directory.
3. Use CMake to build the project.
4. Run the compiled executable.
5. Use Valgrind and thread ... to identify thread ... and memory leaks


   

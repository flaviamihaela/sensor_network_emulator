# Sensor Network Emulator

This project implements a sensor data processing system that simulates the acquisition, processing, and transmission of sensor data from various types of sensors to a central receiver. The system utilizes multithreading, mutexes, condition variables, and design patterns to achieve concurrent and synchronized processing.

## Key aspects:
 - Simulates multiple types of sensor objects: temperature, pressure, and capacitive sensors.
 - Utilizes multithreading ( C++11's thread library ) to acquire sensor data concurrently from multiple threads.
 - Implements a central Bus Controller (BC) for coordinating access to shared resources.
 - Uses a Receiver to collect and process sensor data.
 - Implements a Link Access Controller (LAC) to manage access to communication links.
 - Includes error handling for thread synchronization and resource access.
 - Provides efficient handling of sensor data using deque containers.

## Dependencies
C++11 compiler

## Usage
1. Clone the repository to your local machine.
2. Navigate to the project directory.
3. Use CMake to build the project.
4. Run the compiled executable.

## Future Work
Use CMake for easy build

   

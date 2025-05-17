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
- Uses std::unordered_map to map threads to a human-readable index for log messages - O(1) search time
- Ensures thread synchronization and efficient resource management by using mutexes and condition variables
- Uses the Singleton, Factory and Observer design patterns to make the code more scalable and extendable

## Build 

```bash
cd sensor_network
cd build
cmake ..
cmake --build .
cd ..
.\sensor_network.exe
```

## Dependencies
C++11 compiler
CMake ≥ 3.10
Thread Sanitizer (optional)
Valgrind (optional)

## Usage
1. Clone the repository to your local machine.
2. Navigate to the project directory.
3. Use CMake to build the project.
4. Run the compiled executable.
5. Run Valgrind with a thread-sanitizer to detect race conditions and memory leaks.

# Future work 

1. Group related data to avoid repeated if chains
2. RAII for marking objects (e.g. link) busy/idle
3. Use std::array<std::deque<double>,3> indexed by SensorType - single lock, looped printing, less code, lock-granularity unchanged
   

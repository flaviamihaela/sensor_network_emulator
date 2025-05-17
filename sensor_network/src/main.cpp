#include "globals.hpp"
#include "sensor.hpp"
#include "sensor_data.hpp"
#include "receiver.hpp"
#include "bus_controller.hpp"
#include "utils.hpp"
#include "link.hpp"
#include "link_access_controller.hpp"
#include "thread_task.hpp"

using namespace std;

// Main
int main() {
    std::vector<Sensor*> sensors;

    // Initialise each sensor and insert into vector
    sensors.push_back(MakeSensorFactory(SensorType::temperature_sensor));
    sensors.push_back(MakeSensorFactory(SensorType::pressure_sensor));
    sensors.push_back(MakeSensorFactory(SensorType::capacitive_sensor));

    // Instantiate, start threads
    std::thread t[MAX_NUM_OF_THREADS];
    for (int i = 0; i < MAX_NUM_OF_THREADS; i++) {
        t[i] = std::thread(
            run, std::ref(BC::GetInstance(sensors)), i,
            std::ref(LinkAccessController::GetInstance())); // Launch threads with run function as entry point
    }

    // Wait for threads to finish and join
    for (int i = 0; i < MAX_NUM_OF_THREADS; i++) {
        t[i].join();
    }

    cout << "All threads terminated" << endl;
    
    // Print out number of times each sensor was accessed
    cout << "The temperature sensor was accessed " << counter_ts << " times." << endl;
    cout << "The pressure sensor was accessed " << counter_ps << " times." << endl;
    cout << "The capacitive sensor was accessed " << counter_cs << " times." << endl;

    // Print out all the data in the Receiver
    Receiver::GetInstance().printSensorData();

    // Deallocate memory
    for (Sensor* sensor : sensors) {
        delete sensor;
    }

    return 0;
} 

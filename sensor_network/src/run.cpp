#include "thread_task.hpp"

void run(BC& theBC, int idx, LinkAccessController& lac) {
    // Record thread ID
    {
    std::unique_lock<std::mutex> map_locker(map_mu);
    threadIDs.insert({std::this_thread::get_id(), idx});
    }

    // Each thread collects mix of all data 
    SensorData t_s_data(t_s);
    SensorData p_s_data(p_s);
    SensorData c_s_data(c_s);

    // Acquire sensor data mix using Bus Controller
    acquireSensorData(theBC, idx, t_s_data, p_s_data, c_s_data);

    // Transmit data mix to receiver via Link Access Controller
    transmitSensorData(lac, idx, t_s_data, p_s_data, c_s_data);
}

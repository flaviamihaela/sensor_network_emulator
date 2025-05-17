#include "receiver.hpp"

// Singleton instance getter
Receiver& Receiver::GetInstance() {
    static Receiver s_instance;
    return s_instance;
}

// Private constructor
Receiver::Receiver() {}

// Store incoming sensor data
void Receiver::receiveData(SensorData sd) {
    std::unique_lock<std::mutex> Rec_ul(mtx);
    std::deque<double> sensdata = sd.getSensorData();

    if (sd.getSensorType() == t_s) {
        temp_data.insert(temp_data.end(), sensdata.begin(), sensdata.end());
    }
    if (sd.getSensorType() == p_s) {
        press_data.insert(press_data.end(), sensdata.begin(), sensdata.end());
    }
    if (sd.getSensorType() == c_s) {
        cap_data.insert(cap_data.end(), sensdata.begin(), sensdata.end());
    }
}

// Print all stored sensor values
void Receiver::printSensorData() {
    std::cout << "temperature data:" << std::endl;
    for (const auto& val : temp_data) {
        std::cout << val << std::endl;
    }

    std::cout << "pressure data:" << std::endl;
    for (const auto& val : press_data) {
        std::cout << val << std::endl;
    }

    std::cout << "capacitance data:" << std::endl;
    for (const auto& val : cap_data) {
        std::cout << val << std::endl;
    }
}

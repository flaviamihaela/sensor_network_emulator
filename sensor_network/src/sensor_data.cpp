#include "sensor_data.hpp"

// Constructor
SensorData::SensorData(const std::string& type) : sensor_type(type) {}

// Get sensor type
std::string SensorData::getSensorType() const {
    return sensor_type;
}

// Get full sensor data deque
const std::deque<double>& SensorData::getSensorData() const {
    return sensor_data;
}

// Add new data point
void SensorData::addData(double newData) {
    sensor_data.push_back(newData);
}

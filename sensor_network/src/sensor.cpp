#include "sensor.hpp"
#include "globals.hpp"

// Thread-local distributions for different sensor types
thread_local std::uniform_real_distribution<> temp_dist(10.0, 30.0);
thread_local std::uniform_real_distribution<> press_dist(95.0, 105.0);
thread_local std::uniform_real_distribution<> cap_dist(1.0, 5.0);

// Sensor base constructor
Sensor::Sensor(const std::string& type) : sensorType(type) {}

std::string Sensor::getType() const {
    return sensorType;
}

// Derived: TempSensor
TempSensor::TempSensor(const std::string& s) : Sensor(s) {}
double TempSensor::getValue() {
    return temp_dist(gen);
}

// Derived: PressureSensor
PressureSensor::PressureSensor(const std::string& s) : Sensor(s) {}
double PressureSensor::getValue() {
    return press_dist(gen);
}

// Derived: CapacitiveSensor
CapacitiveSensor::CapacitiveSensor(const std::string& s) : Sensor(s) {}
double CapacitiveSensor::getValue() {
    return cap_dist(gen);
}

// Factory function implementation
Sensor* MakeSensorFactory(SensorType type) {
    if (type == SensorType::temperature_sensor) {
        return new TempSensor(t_s);
    } else if (type == SensorType::pressure_sensor) {
        return new PressureSensor(p_s);
    } else if (type == SensorType::capacitive_sensor) {
        return new CapacitiveSensor(c_s);
    }
    return nullptr;
}

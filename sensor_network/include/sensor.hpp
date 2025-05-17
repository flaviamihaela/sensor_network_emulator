#pragma once

#include <string>
#include <random>

extern thread_local std::mt19937 gen;
extern std::string t_s, p_s, c_s;

class Sensor {
    public:
        explicit Sensor(const std::string& type);
        virtual ~Sensor() = default;
        virtual double getValue() = 0;
        std::string getType() const;

    protected:
        std::string sensorType;
};

class TempSensor : public Sensor {
    public:
        explicit TempSensor(const std::string& s);
        double getValue() override;
};

class PressureSensor : public Sensor {
    public:
        explicit PressureSensor(const std::string& s);
        double getValue() override;
};

class CapacitiveSensor : public Sensor {
    public:
        explicit CapacitiveSensor(const std::string& s);
        double getValue() override;
};

enum class SensorType {
    temperature_sensor,
    pressure_sensor,
    capacitive_sensor
};

Sensor* MakeSensorFactory(SensorType type);

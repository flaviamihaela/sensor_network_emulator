#pragma once

#include <string>
#include <deque>

class SensorData {
    public:

        explicit SensorData(const std::string& type);

        std::string getSensorType() const;
        const std::deque<double>& getSensorData() const;

        void addData(double newData);

    private:
        std::string sensor_type = "";
        std::deque<double> sensor_data; // Stores data samples
};

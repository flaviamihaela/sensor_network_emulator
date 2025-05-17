#pragma once

#include "sensor_data.hpp"
#include "globals.hpp"
#include <deque>
#include <mutex>
#include <iostream>

class Receiver {
    public:
        static Receiver& GetInstance();

        void receiveData(SensorData sd);

        void printSensorData();

    private:
        Receiver();

        // Disable copy & move
        Receiver(const Receiver&) = delete;
        Receiver& operator=(const Receiver&) = delete;

        std::mutex mtx;
        std::deque<double> temp_data;
        std::deque<double> press_data;
        std::deque<double> cap_data;
};

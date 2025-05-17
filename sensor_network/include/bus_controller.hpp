#pragma once

#include "sensor.hpp"
#include "globals.hpp" 
#include <vector>
#include <string>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <sstream>

class BC {
    public:
        static BC& GetInstance(std::vector<Sensor*>& sensors);
        void requestBC();
        double getSensorValue(int selector);
        std::string getSensorType(int selector);
        void releaseBC();

    private:
        explicit BC(std::vector<Sensor*>& sensors);

        // Disable copy & move
        BC(const BC&) = delete;
        BC& operator=(const BC&) = delete;

        std::atomic<bool> lock{false};
        std::vector<Sensor*>& theSensors;
        std::mutex BC_mu;
        std::condition_variable vc;
};

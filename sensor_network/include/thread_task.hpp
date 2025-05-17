#pragma once

#include "bus_controller.hpp"
#include "link_access_controller.hpp"
#include "sensor_data.hpp"
#include "utils.hpp"
#include "globals.hpp"
#include <thread>
#include <chrono>
#include <mutex>
#include <sstream>
#include <random>

void acquireSensorData(BC& theBC, int idx, SensorData& t_s_data, SensorData& p_s_data, SensorData& c_s_data);
void transmitSensorData(LinkAccessController& lac, int idx, const SensorData& t_s_data, const SensorData& p_s_data, const SensorData& c_s_data);
void run(BC& theBC, int idx, LinkAccessController& lac);

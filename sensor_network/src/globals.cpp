#include "globals.hpp"

// Thread-local random number generator
thread_local std::mt19937 gen(std::random_device{}());

// Thread-local uniform distribution
thread_local std::uniform_int_distribution<> dis(1, 1000);

// Global mutex for threadID map
std::mutex map_mu;

// Simulation parameters
const int MAX_NUM_OF_THREADS = 6;   // No. threads to simulate
const int NUM_OF_LINKS = 2;         // No. available communication links
const int NUM_OF_SAMPLES = 50;      // No. data samples collected per thread

// Global strings to identify each sensor type
std::string t_s = "temperature_sensor";
std::string p_s = "pressure_sensor";
std::string c_s = "capacitive_sensor";

// Atomic counters to track no. times each sensor was accessed
std::atomic<int> counter_ts{0};
std::atomic<int> counter_ps{0};
std::atomic<int> counter_cs{0};

// Global map storing thread IDs and thread indices
std::unordered_map<std::thread::id, int> threadIDs;

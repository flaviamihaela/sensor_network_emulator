#pragma once

#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <atomic>

extern thread_local std::mt19937 gen;
extern thread_local std::uniform_int_distribution<> dis;
extern std::mutex map_mu;

extern const int MAX_NUM_OF_THREADS;
extern const int NUM_OF_LINKS;
extern const int NUM_OF_SAMPLES;

extern std::string t_s, p_s, c_s;

extern std::atomic<int> counter_ts;
extern std::atomic<int> counter_ps;
extern std::atomic<int> counter_cs;

extern std::unordered_map<std::thread::id, int> threadIDs;
#pragma once

#include <string>
#include <mutex>
#include <iostream>
#include <unordered_map>
#include <thread>

extern std::unordered_map<std::thread::id, int> threadIDs;
extern std::mutex map_mu;

void printing(const std::string& str);

int search();

#include "utils.hpp"
#include "globals.hpp"

// Thread-safe print function
void printing(const std::string& str) {
  static std::mutex m;
  std::lock_guard<std::mutex> lg(m);
  std::cout << str << std::endl;
}

// Look up thread ID in the map
int search() {
  std::unique_lock<std::mutex> lock(map_mu);
  auto it = threadIDs.find(std::this_thread::get_id());
  return (it == threadIDs.end()) ? -1 : it->second;
}

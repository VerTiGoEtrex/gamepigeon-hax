#include "timeout.hpp"

bool TimeOut::keepWorking() {
  auto keepWork = start + timeout > std::chrono::steady_clock::now();
  // std::cout << "keep working? " << keepWork << std::endl;
  return keepWork;
}
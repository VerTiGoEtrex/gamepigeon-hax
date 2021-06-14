#pragma once
#include <chrono>
#include <iostream>

struct TimeOut {
  std::chrono::steady_clock::time_point start =
      std::chrono::steady_clock::now();
  std::chrono::milliseconds timeout;
  TimeOut(std::chrono::milliseconds t) : timeout(t) {}

  bool keepWorking();
};
// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
#ifndef APPCALCULATOR_HPP
#define APPCALCULATOR_HPP

#include <iostream>
#include <vector>

class AppCalculator {
 public:
  virtual size_t processNumber(int64_t number, std::vector<int64_t>&nums) = 0;
};

#endif  // APPCALCULATOR_HPP

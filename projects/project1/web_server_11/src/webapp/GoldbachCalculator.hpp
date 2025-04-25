// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
#ifndef GOLDBACHCALCULATOR_HPP
#define GOLDBACHCALCULATOR_HPP

#include <iostream>
#include <vector>
#include "Prime.hpp"

class GoldbachSums {
 public:
  size_t processNumber(int64_t number, std::vector<int64_t>&goldbachSums);

  size_t getNSums(std::vector<int64_t>&goldbachSums, int64_t nElements);

 private:
  bool strongGoldbach(int64_t number, std::vector<int64_t>&goldbachSums);

  bool weakGoldbach(int64_t number, std::vector<int64_t>&goldbachSums);
};

#endif

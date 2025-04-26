// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef PRIMEFACTCALCULATOR_HPP
#define PRIMEFACTCALCULATOR_HPP

#include <inttypes.h>
#include <math.h>
#include <vector>

#include "Prime.hpp"

class PrimeFactCalculator {
 public:
  size_t processNumber(int64_t number, std::vector<int64_t>& primeFactors);

 private:
  int64_t divideWhileDivisible(int64_t& number, int64_t factor);
  void addPrimeFactor(int64_t& number, int64_t primeFactor
      , std::vector<int64_t>& primeFactors);
};

#endif  // PRIMEFACTCALCULATOR_HPP

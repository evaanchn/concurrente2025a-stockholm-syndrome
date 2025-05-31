// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include <vector>

#include "GoldbachCalculator.hpp"

size_t GoldbachCalculator::processNumber(int64_t number,
    std::vector<int64_t>& goldbachSums) {
    number = std::abs(number);  // Ensure the number is positive
    int64_t sumOperands = 1;
    if (number % 2 == 0) {
      this->strongGoldbach(number, goldbachSums);
      sumOperands = 2;

    } else {
      this->weakGoldbach(number, goldbachSums);
      sumOperands = 3;
    }
    return getNSums(goldbachSums, sumOperands);
}

bool GoldbachCalculator::strongGoldbach(int64_t number,
    std::vector<int64_t>&goldbachSums) {
  bool found = false;
  for (int64_t i = 2; i <= number / 2; ++i) {
    if (Prime::isPrime(i) && Prime::isPrime(number - i)) {
      goldbachSums.insert(goldbachSums.end(), {i, number-i});
      found = true;
    }
  }
  return found;
}

bool GoldbachCalculator::weakGoldbach(int64_t number,
    std::vector<int64_t>&goldbachSums) {
  bool found = false;
  for (int i = 2; i <= number / 3; ++i) {
    if (!Prime::isPrime(i)) continue;
    for (int64_t j = i; j <= (number - i) / 2; ++j) {
      if (!Prime::isPrime(j)) continue;
      int64_t k = number - i - j;
      if (k < j) continue;
      if (Prime::isPrime(k)) {
        goldbachSums.insert(goldbachSums.end(), {i, j, k});
        found = true;
      }
    }
  }
  return found;
}

size_t GoldbachCalculator::getNSums(std::vector<int64_t>&goldbachSums,
    int64_t nElements) {
  return (goldbachSums.size())/nElements;
}

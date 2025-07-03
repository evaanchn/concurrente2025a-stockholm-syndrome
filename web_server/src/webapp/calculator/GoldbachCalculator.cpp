// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include <vector>
#include <cstdlib>

#include "GoldbachCalculator.hpp"
#include "Prime.hpp"

// Main processing function that routes to appropriate Goldbach conjecture
size_t GoldbachCalculator::processNumber(int64_t number,
    std::vector<int64_t>& goldbachSums) {
    number = std::abs(number);  // Ensure the number is positive
    int64_t sumOperands = 1;
    // Even numbers use strong Goldbach conjecture (sum of 2 primes)
    if (number % 2 == 0) {
      this->strongGoldbach(number, goldbachSums);
      sumOperands = 2;  // Each sum consists of 2 numbers
      // Odd numbers use weak Goldbach conjecture (sum of 3 primes)
    } else {
      this->weakGoldbach(number, goldbachSums);
      sumOperands = 3;  // Each sum consists of 3 numbers
    }
    return this->getNSums(goldbachSums, sumOperands);
}

// Implements strong Goldbach conjecture for even numbers (n = p1 + p2)
bool GoldbachCalculator::strongGoldbach(int64_t number,
    std::vector<int64_t>&goldbachSums) {
  bool found = false;
  // Check all possible prime pairs that sum to the number
  for (int64_t i = 2; i <= number / 2; ++i) {
    // If both i and (number-i) are primes, we found a valid pair
    if (Prime::isPrime(i) && Prime::isPrime(number - i)) {
      goldbachSums.insert(goldbachSums.end(), {i, number-i});
      found = true;
    }
  }
  return found;
}

// Implements weak Goldbach conjecture for odd numbers (n = p1 + p2 + p3)
bool GoldbachCalculator::weakGoldbach(int64_t number,
    std::vector<int64_t>&goldbachSums) {
  bool found = false;
  // Triple nested loop would be too slow, so we use a smarter approach:
  // Fix first prime (i), then find two primes (j,k) that sum to (number-i)
  for (int i = 2; i <= number / 3; ++i) {
    if (!Prime::isPrime(i)) continue;  // Skip non-prime i values
    for (int64_t j = i; j <= (number - i) / 2; ++j) {
      if (!Prime::isPrime(j)) continue;  // Skip non-prime j values
      int64_t k = number - i - j;
      if (k < j) continue;  // Maintain i <= j <= k ordering to avoid duplicates
      if (Prime::isPrime(k)) {
        goldbachSums.insert(goldbachSums.end(), {i, j, k});
        found = true;
      }
    }
  }
  return found;
}

// Calculates number of complete sums based on elements per sum
size_t GoldbachCalculator::getNSums(std::vector<int64_t>&goldbachSums,
    int64_t nElements) {
  // Simple division gives count of sums
  return (goldbachSums.size())/nElements;
}

// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef PRIMEFACTCALCULATOR_HPP
#define PRIMEFACTCALCULATOR_HPP

#include <inttypes.h>
#include <math.h>
#include <vector>

#include "Prime.hpp"

class PrimeFactCalculator {
 public:
  /**
   * Processes a number to find all its prime factors and their exponents.
   * @param number The number to factorize.
   * @param primeFactors Vector to store pairs of prime factors
   * and their exponents.
   * @return The count of distinct prime factors.
   */
  size_t processNumber(int64_t number, std::vector<int64_t>& primeFactors);

 private:
  /**
   * Divides the number by a given factor as many times as possible.
   * @param number Reference to the number to divide.
   * @param factor The divisor to apply repeatedly.
   * @return The number of times the division was successful (the exponent).
   */
  int64_t divideWhileDivisible(int64_t& number, int64_t factor);

  /**
   * Adds a prime factor and its exponent to the primeFactors vector.
   * @param number Reference to the number being factorized.
   * @param primeFactor The prime factor to add.
   * @param primeFactors Vector where the factor and its count are stored.
   */
  void logPrimeFactor(int64_t& number, int64_t primeFactor
    , std::vector<int64_t>& primeFactors);
};

#endif  // PRIMEFACTCALCULATOR_HPP

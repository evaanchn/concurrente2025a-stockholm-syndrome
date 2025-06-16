// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef GOLDBACHCALCULATOR_HPP
#define GOLDBACHCALCULATOR_HPP

#include <vector>
#include <cstddef>
#include <cinttypes>

#include "AppCalculator.hpp"

/**
 * @class GoldbachCalculator
 * @brief Calculates the Goldbach sums for a given number.
 *
 * Provides methods to process a number according to Goldbach's conjecture
 * and retrieve the generated sums.
 */
class GoldbachCalculator: public AppCalculator{
 public:
  /**
  * @brief Processes a number to find its Goldbach sums.
  * 
  * This method fills the provided vector with the prime components
  * that sum up to the given number according to Goldbach's conjectures.
  *
  * @param number The number to process.
  * @param goldbachSums A reference to the vector where the results will be stored.
  * @return The total number of sums found.
  */
  size_t processNumber(int64_t number, std::vector<int64_t>&goldbachSums);

 private:
   /**
  * @brief Applies the strong Goldbach conjecture to a number.
  *
  * Finds two prime numbers that sum to the given even number.
  *
  * @param number The even number to process.
  * @param goldbachSums A reference to the vector where the result will be stored.
  * @return True if a valid sum was found, false otherwise.
  */
  bool strongGoldbach(int64_t number, std::vector<int64_t>&goldbachSums);
  /**
  * @brief Applies the weak Goldbach conjecture to a number.
  *
  * Finds three prime numbers that sum to the given odd number.
  *
  * @param number The odd number to process.
  * @param goldbachSums A reference to the vector where the result will be stored.
  * @return True if a valid sum was found, false otherwise.
  */
  bool weakGoldbach(int64_t number, std::vector<int64_t>&goldbachSums);

  /**
  * @brief Retrieves the number of complete Goldbach sums.
  *
  * Calculates how many sums are stored in the given vector,
  * based on the expected number of elements per sum.
  *
  * @param goldbachSums The vector containing the sums.
  * @param nElements Number of elements that form a complete sum.
  * @return The number of complete sums.
  */
  size_t getNSums(std::vector<int64_t>&goldbachSums, int64_t nElements);
};

#endif  // GOLDBACHCALCULATOR_HPP

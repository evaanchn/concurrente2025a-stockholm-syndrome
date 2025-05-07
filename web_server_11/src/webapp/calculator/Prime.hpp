// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
#ifndef PRIME_HPP
#define PRIME_HPP

#include <cinttypes>

/**
 * @class Prime
 * @brief Provides functionality to check whether a number is prime.
 *
 * This class offers a static method to verify if a given number is prime.
 */
class Prime {
 public:
   /**
   * Checks if a given number is a prime number.
   * @param number A 64-bit integer to be checked if it is prime.
   * @return true if the number is prime, false otherwise.
   */
  static bool isPrime(int64_t number);
};
#endif

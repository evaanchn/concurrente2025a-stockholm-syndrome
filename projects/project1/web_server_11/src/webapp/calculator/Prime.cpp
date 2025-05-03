// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "Prime.hpp"

bool Prime::isPrime(int64_t number) {
  if (number <= 1) return false;
  if (number == 2) return true;
  if (number % 2 == 0) return false;
  for (int factor = 3; factor * factor <= number; factor += 2)
    if (number % factor == 0) return false;
  return true;
}

// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "PrimeFactCalculator.hpp"

size_t PrimeFactCalculator::processNumber(int64_t number
    , std::vector<int64_t>& primeFactors) {
  if (number <= 1) return 0;
  int64_t primeFactorsCount = 0;

  // Takes care of the only even prime 2 if even, to increment by odds by two
  if (number % 2 == 0) {
    logPrimeFactor(number, 2, primeFactors);
    ++primeFactorsCount;
  }

  // Evaluate following odd primes
  // for factor from 3 to sqrt(number) step 2 do
  for (int64_t factor = 3; factor < sqrt(number); factor += 2) {
    //   if mod(number, factor) != 0 or not isPrime(factor) then
    if (number % factor != 0 || !Prime::isPrime(factor)) {
      continue;  // No need to add non-factors or non-prime factors
    }  // end if

    logPrimeFactor(number, factor, primeFactors);
    ++primeFactorsCount;
  }  // end for

  // If remaining number (or orignal number) > 1, it's a prime and thus a factor
  // if number > 1 then
  if (number > 1) {
    // factors[number] := 1
    primeFactors.push_back(number);
    primeFactors.push_back(1);  // Only one of this factor
    ++primeFactorsCount;
  }  // end if

  return primeFactorsCount;
}

int64_t PrimeFactCalculator::divideWhileDivisible(int64_t& number
    , int64_t factor) {
  // primeFactorCount := 0
  int64_t primeFactorCount = 0;

  // while mod(number, factor) = 0 do
  while (number % factor == 0) {
    // number := number / factor
    number /= factor;
    // primeCount := primeCount + 1
    ++primeFactorCount;
  }  // end while

  return primeFactorCount;
}

void PrimeFactCalculator::logPrimeFactor(int64_t& number, int64_t primeFactor
    , std::vector<int64_t>& primeFactors) {
  int64_t primeFactorsCount = divideWhileDivisible(number, primeFactor);
  primeFactors.push_back(primeFactor);
  primeFactors.push_back(primeFactorsCount);
}

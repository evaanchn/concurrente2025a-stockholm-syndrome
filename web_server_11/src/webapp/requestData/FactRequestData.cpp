// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "FactRequestData.hpp"

void FactRequestData::processQuery(size_t index) {
  PrimeFactCalculator calculator;
  // processNumber(query, resultsVector)
  calculator.processNumber(this->queries[index], this->results[index]);
}

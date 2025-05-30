// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include <vector>

#include "GoldbachRequestData.hpp"

void GoldbachRequestData::processQuery(size_t index) {
  GoldbachCalculator calculator;
  // processNumber(query, resultsVector)
  try {
    calculator.processNumber(this->queries[index], this->results[index]);
  } catch(const std::exception& error) {
    // respond that no goldbach sums where found for the given number
    this->httpResponse.body()
      << "    <li class=err>"
      <<        error.what()
      << "    </li>";
  }
}

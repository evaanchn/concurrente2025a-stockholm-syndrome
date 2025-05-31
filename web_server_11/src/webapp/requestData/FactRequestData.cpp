// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "FactRequestData.hpp"

void FactRequestData::processQuery(size_t index) {
  PrimeFactCalculator calculator;
  // processNumber(query, resultsVector)
  calculator.processNumber(this->queries[index], this->results[index]);
}

void FactRequestData::buildResult(int64_t value
  , std::vector<int64_t>& result) {
  // result contains each prime factor and its exponent
  // e.g: {prime1, exponent1, prime2, exponent2, ...}
  size_t primeFactorsCount = result.size() / 2;
  this->httpResponse.body()
    << "        <div class='number-result'>\n"
    << "          <span class='number-value'>" << value << "</span> = \n"
    << "          <span class='factorization'>";
  if (primeFactorsCount == 0) {
    this->httpResponse.body() << "<span class='err'>Invalid number</span>";
  } else {
    size_t lastFactor = result.size() - 2;
    for (size_t factIndex = 0; factIndex < primeFactorsCount * 2;
        factIndex += 2) {
      this->httpResponse.body()
        << "<span class='factor'>" << result.at(factIndex) << "</span>";
      if (result.at(factIndex + 1) != 1) {
        this->httpResponse.body()
          << "<sup class='exponent'>"
          << result.at(factIndex + 1)
          << "</sup>";
      }
      // Avoid printing a multiplication symbol after last prime factor
      if (factIndex < lastFactor) {
        this->httpResponse.body() << " × ";
      }
    }
  }

  this->httpResponse.body()
    << "          </span>\n"
    << "        </div>\n";
}

// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "FactWebApp.hpp"

FactWebApp::FactWebApp()
: CalcWebApp("/fact", "/fact?number=", "Prime Factorization") {
}

void FactWebApp::buildResponse(const int64_t value
  , HttpResponse& httpResponse) {
  // array with the format: {count, factor1, exponent1, ..., factorNVal,
  // exponentNVal}. factResponse is encharged of manage the number of operands
  // for each factorization
  std::vector<int64_t> primeFactors;
  int64_t primeFactorsCount = 0;
  PrimeFactCalculator myPrimeFact;
  primeFactorsCount = myPrimeFact.processNumber(std::abs(value), primeFactors);
  // No factors were found for value
  if (primeFactorsCount == 0) {
    // if true for an even number this could demostrate Goldbach strong
    // conjecture as false.
    httpResponse.body() << "    <li class=err>" << value
      << ": Invalid value for this program\n";
    return;
  } else {
    factResponse(primeFactors, value, primeFactorsCount, httpResponse);
  }
}

void FactWebApp::factResponse(std::vector<int64_t>& primeFactors
  , const int64_t value, int64_t primeFactorsCount
  , HttpResponse& httpResponse) {
  // Creates a list
  httpResponse.body()
    << "    <li>\n" << value << " = ";
  // Prints all factors
  for (int64_t factIndex = 0; factIndex < primeFactorsCount * 2
    ; factIndex += 2) {
    httpResponse.body() << primeFactors.at(factIndex);
    // Check if the exponent is 1 to not print it
    if (primeFactors.at(factIndex + 1) != 1) {
      httpResponse.body()
        << "<sup>" << primeFactors.at(factIndex + 1) << "</sup>";
    }
    // Check if it is necessary to print an * operator
    if (factIndex < (primeFactorsCount * 2) - 2) {
      httpResponse.body() << " * ";
    }
  }
  httpResponse.body()
    << "    </li>\n";
}

// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "GoldbachWebApp.hpp"

GoldbachWebApp::GoldbachWebApp()
: CalcWebApp("/goldbach", "/goldbach?number=", "Goldbach sums") {
}

void GoldbachWebApp::buildResponse(const int64_t value
    , HttpResponse& httpResponse) {
  // array with the format: {value, sum1val, sum1val, ..., sumNval, sumNval}
  // sumsResponse is encharged of manage the number of operands for each sum
  std::vector<int64_t> sums;
  int64_t sumsCount = 0;
  GoldbachCalculator myGoldbach;
  sumsCount = myGoldbach.processNumber(std::abs(value), sums);
  // No sums were found for value
  if (sumsCount == 0) {
    // if true for an even number this could demostrate Goldbach strong
    // conjecture as false.
    httpResponse.body() << "    <li class=err>" << value
      << ": no goldbach sums found\n";
    return;
  }
  if (value < 0) {  // get sums count
    this->sumsCountResponse(std::abs(value), sumsCount, httpResponse);
  } else {  // get sums list
    this->sumsResponse(sums, httpResponse);
  }
}

void GoldbachWebApp::sumsCountResponse(int64_t value, int64_t sumsCount
    , HttpResponse& httpResponse) {
  httpResponse.body()
    << "    <li>" << value << " has: " << sumsCount << " goldbach sums\n";
}

void GoldbachWebApp::sumsResponse(std::vector<int64_t>& sums
    , HttpResponse& httpResponse) {
  // count of operands for each sum
  size_t sumOperands = 0;
  // even
  if ((sums.front() % 2) == 0) {
    sumOperands = 2;
  // odd
  } else {
    sumOperands = 3;
  }
  // Sums as bulleted list
  httpResponse.body()
    << "    <li>" << sums.front() << " sums:\n"
    << "      <ul>";
  for (size_t sumIndex = 1; sumIndex <= sums.size() - sumOperands
      ; sumIndex+= sumOperands) {
    // sum
    httpResponse.body() << "        <li>";
    for (size_t operandIndex = sumIndex; operandIndex < sumOperands + sumIndex
      // operands concatenation
        ; ++operandIndex) {
      httpResponse.body() << sums.at(operandIndex);
      if (operandIndex != sumOperands + sumIndex - 1)
        httpResponse.body() << " + ";
    }
    httpResponse.body() << "        </li>\n";
  }
  httpResponse.body()
    << "      </ul>\n"
    << "    </li>\n";
}

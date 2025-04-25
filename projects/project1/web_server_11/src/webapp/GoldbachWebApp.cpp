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
  sums.push_back(value);
  int64_t sumsCount = 0;
  // sumsCount = this->goldbach.sums(sums);
  // No sums were found for value
  if (sumsCount == 0) {
    // if true for an even number this could demostrate Goldbach strong
    // conjecture as false.
    httpResponse.body() << "    <li class=err>" << value
    << ": no goldbach sums found\n";
    return;
  }
  if (value < 0) {
    this->sumsCountResponse(value, sumsCount, httpResponse);
  } else {
    this->sumsResponse(sums, httpResponse);
  }
}

void GoldbachWebApp::sumsCountResponse(int64_t value, int64_t sumsCount
    , HttpResponse& httpResponse) {
  httpResponse.body()
    << "    <li>" << value << "has: " << sumsCount << "goldbach sums\n";
}

void GoldbachWebApp::sumsResponse(std::vector<int64_t>& sums
    , HttpResponse& httpResponse) {
  // this->goldbach.sums(sums);
  httpResponse.body()
    << "    <li>" << sums.front() << " sums:\n"
    << "      <ul>";
  // TODO(me): different list for even and odd numbers
  httpResponse.body()
    << "      </ul>\n"
    << "    </li>\n";
}

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
      << this->queries[index] << ": " << error.what()
      << "    </li>";
  }
}

void GoldbachRequestData::buildResult(std::vector<int64_t>& result
  , HttpResponse& httpResponse) {
  // checks wheter each sum() has two (even value) or three(odd) operands
  size_t sumOperands = (result.front() % 2 == 0) ? 2 : 3;
  // result contains the value at the first position and is followed by its
  // sums
  size_t sumsCount = (result.size() - 1) / sumOperands;
  httpResponse.body()
    << "        <div class='number-result'>\n"
    << "          <span class='number-value'>" << result.front()
    << "</span>: \n"
    << "          <span class='sums-count'>" << sumsCount << " sums</span>\n"
    << "        </div>";
  if (result.front() < 0 && sumsCount > 0) {
    this->sumsResponse(result, httpResponse);
  }
}

void GoldbachRequestData::sumsResponse(std::vector<int64_t>& result
  , HttpResponse& httpResponse) {
  size_t sumOperands = ((result.front() % 2) == 0) ? 2 : 3;
  httpResponse.body()
    << "        <div class='sums-grid'>";
  for (size_t sumIndex = 1; sumIndex <= result.size() - sumOperands;
    sumIndex += sumOperands) {
    httpResponse.body()
      << "          <div class='sum-item'>";
    for (size_t operandIndex = sumIndex;
      operandIndex < sumOperands + sumIndex; ++operandIndex) {
      httpResponse.body() << result.at(operandIndex);
      if (operandIndex != sumOperands + sumIndex - 1) {
        httpResponse.body() << " + ";
      }
    }
    httpResponse.body() << "          </div>";
  }
  httpResponse.body() << "        </div>";
}

// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include <vector>

#include "GoldbachRequestData.hpp"

void GoldbachRequestData::processQuery(size_t index) {
  GoldbachCalculator calculator;
  // processNumber(query, resultsVector)
  calculator.processNumber(this->queries[index], this->results[index]);
}

void GoldbachRequestData::buildResult(int64_t value
  , std::vector<int64_t>& result) {
  // checks wheter each sum() has two (even value) or three(odd) operands
  size_t sumOperands = ((std::abs(value) % 2 == 0) ? 2 : 3);
  // result contains each sum operands adjacent to each other
  // e.g: {sum1val, sum1val, sum2val, sum2val, ...}
  size_t sumsCount = (result.size()) / sumOperands;
  this->httpResponse.body()
    << "        <div class='number-result'>\n"
    << "          <span class='number-value'>" << value
    << "</span>: \n"
    << "          <span class='sums-count'>" << sumsCount << " sums</span>\n"
    << "        </div>";
  if (value < 0 && sumsCount > 0) {
    this->sumsResponse(sumOperands, result);
  }
}

void GoldbachRequestData::sumsResponse(size_t sumOperands
    , std::vector<int64_t>& result) {
  this->httpResponse.body()
    << "        <div class='sums-grid'>";
  // Sums are grouped by two or three operands
  for (size_t sumIndex = 0; sumIndex <= result.size() - sumOperands;
    sumIndex += sumOperands) {
    this->httpResponse.body()
      << "          <div class='sum-item'>";
    // Sum operands
    for (size_t operandIndex = sumIndex;
      operandIndex < sumOperands + sumIndex; ++operandIndex) {
      this->httpResponse.body() << result.at(operandIndex);
      if (operandIndex != sumOperands + sumIndex - 1) {
        this->httpResponse.body() << " + ";
      }
    }
    this->httpResponse.body() << "          </div>";
  }
  this->httpResponse.body() << "        </div>";
}

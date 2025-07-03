// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include <vector>

#include "GoldbachCalculator.hpp"
#include "GoldbachData.hpp"

void GoldbachData::processQuery(size_t index) {
  GoldbachCalculator calculator;
  // processNumber(query, resultsVector)
  size_t sums_count =
      calculator.processNumber(this->queries[index], this->results[index]);
  // If query is positive, only add number of sums to result
  // This prevents workers from having to send too many values.
  if (this->queries[index] >= 0) {
    this->results[index].clear();  // Clear registered sums
    // Only insert sum
    this->results[index].push_back(sums_count);
  }
}

void GoldbachData::buildResult(int64_t value
  , std::vector<int64_t>& result) {
  // checks wheter each sum() has two (even value) or three(odd) operands
  size_t sumOperands = ((std::abs(value) % 2 == 0) ? 2 : 3);
  // result contains each sum operands adjacent to each other
  // e.g: {sum1val, sum1val, sum2val, sum2val, ...}
  size_t sumsCount = (result.size()) / sumOperands;
  // If request was positive, response will only have sums count 
  if (value >= 0) sumsCount = result[0];
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

void GoldbachData::sumsResponse(size_t sumOperands
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

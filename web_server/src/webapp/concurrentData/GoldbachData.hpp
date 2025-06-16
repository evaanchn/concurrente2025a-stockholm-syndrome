// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef GOLDBACH_REQUESTDATA_HPP
#define GOLDBACH_REQUESTDATA_HPP

#include <iostream>
#include <vector>

#include "CalcRequestData.hpp"

/// @brief Store and process data for goldbach app
class GoldbachRequestData : public CalcRequestData {
 public:
  /// Constructor
  GoldbachRequestData(HttpRequest& httpRequest, HttpResponse& httpRespomse)
      : CalcRequestData(httpRequest, httpRespomse) {
  }
  /// @brief Process a query for goldbach sums
  /// @param index index of the queries to process
  void processQuery(size_t index) override;
  /// @brief Goldbach sums for a given value
  /// @param value the original calculation value
  /// @param result vector containing the result elements
  void buildResult(int64_t value, std::vector<int64_t>& result)
    override;
  /// @brief List the possible goldbach sums of value
  /// @param sumOperands number of operands for each goldbach sum
  /// @param result results of goldbach sum: {sum1val, sum1val, ...}
  void sumsResponse(size_t sumOperands, std::vector<int64_t>& result);
};

#endif  // GOLDBACH_REQUESTDATA_HPP

// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef GOLDBACHDATA_HPP
#define GOLDBACHDATA_HPP

#include <iostream>
#include <vector>

#include "CalcData.hpp"

/// @brief Store and process data for goldbach app
class GoldbachData : public CalcData {
 public:
  /// Constructor
  GoldbachData(HttpRequest& httpRequest, HttpResponse& httpRespomse,
    const size_t appIndex):
      CalcData(httpRequest, httpRespomse, appIndex) {
  }
  /// @brief Constructor for workers
  /// @param appIndex Index of app that created the data
  explicit GoldbachData(const size_t appIndex): CalcData(appIndex) {
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

#endif  // GOLDBACHDATA_HPP

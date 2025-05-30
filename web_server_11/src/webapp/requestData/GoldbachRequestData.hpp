// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef GOLDBACH_REQUESTDATA_HPP
#define GOLDBACH_REQUESTDATA_HPP

#include <iostream>

#include "GoldbachCalculator.hpp"
#include "ConcurrentApp.hpp"
#include "CalcRequestData.hpp"

/// @brief Base class to save data from a client request throughout the
/// concurrent production line
class GoldbachRequestData : public CalcRequestData {
 public:
  /// Constructor
  GoldbachRequestData(HttpRequest& httpRequest): CalcRequestData(httpRequest) {
  }
  /// @brief Process a query for goldbach sums
  /// @param index index of the queries to process
  void processQuery(size_t index) override;
  /// @brief Goldbach sums for a given value
  /// @param result vector containing the original value and its sums
  /// @param httpResponse The object to answer to the client/user
  void buildResult(std::vector<int64_t>& result
    , HttpResponse& httpResponse) override;
  /// @brief List the possible goldbach sums of value
  /// @param result results of goldbach sum: {value, sum1val, sum1val, ...}
  /// @param httpResponse The object to answer to the client/user
  void sumsResponse(std::vector<int64_t>& result, HttpResponse& httpResponse);
};

#endif  // GOLDBACH_REQUESTDATA_HPP

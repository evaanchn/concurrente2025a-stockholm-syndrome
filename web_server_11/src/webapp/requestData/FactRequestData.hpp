// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef FACT_REQUESTDATA_HPP
#define FACT_REQUESTDATA_HPP

#include <iostream>
#include <vector>

#include "PrimeFactCalculator.hpp"
#include "ConcurrentApp.hpp"
#include "CalcRequestData.hpp"

/// @brief Base class to save data from a client request throughout the
/// concurrent production line
class FactRequestData : public CalcRequestData {
 public:
  /// @brief Constructor
  /// @param httpRequest is the request to be handled
  /// @param httpResponse is the response to be handled
  FactRequestData(HttpRequest& httpRequest, HttpResponse& httpResponse)
      : CalcRequestData(httpRequest, httpResponse) {
  }
  /// @brief Process a queries for prime factorization
  /// @param index index of the queries to process
  void processQuery(size_t index) override;
  /// @brief Prime factorization for a given value
  /// @param value the original calculation value
  /// @param result vector containing the result elements
  void buildResult(int64_t value, std::vector<int64_t>& result)
    override;
};

#endif  // FACT_REQUESTDATA_HPP

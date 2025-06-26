// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef PRIMEFACTDATA_HPP
#define PRIMEFACTDATA_HPP

#include <iostream>
#include <vector>

#include "CalcData.hpp"

/// @brief Store and process data for Fact app
/// concurrent production line
class PrimeFactData : public CalcData {
 public:
  /// @brief Constructor
  /// @param httpRequest is the request to be handled
  /// @param httpResponse is the response to be handled
  /// @param appIndex index of the application that created this dats
  PrimeFactData(HttpRequest& httpRequest, HttpResponse& httpResponse
    , const size_t appIndex)
  : CalcData(httpRequest, httpResponse, appIndex) {
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

#endif  // PRIMEFACTDATA_HPP

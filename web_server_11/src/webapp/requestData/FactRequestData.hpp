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
  /// @param concurrentApp is the application that will handle the request
  FactRequestData(HttpRequest& httpRequest): CalcRequestData(httpRequest) {
  }
  /// @brief Process a queries for prime factorization
  /// @param index index of the queries to process
  void processQuery(size_t index) override;
  /// @brief Prime factorization for a given value
  /// @param result vector containing the original value and its prime factors
  /// @param httpResponse The object to answer to the client/user
  void buildResult(std::vector<int64_t>& result, HttpResponse& httpResponse)
    override;
};

#endif  // FACT_REQUESTDATA_HPP

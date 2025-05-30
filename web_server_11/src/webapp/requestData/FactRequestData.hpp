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
  FactRequestData(HttpRequest& httpRequest, ConcurrentApp* concurrentApp)
  : CalcRequestData(httpRequest, concurrentApp) {
  }

  /// @brief Process a queries for prime factorization
  /// @param index index of the queries to process
  void processQuery(size_t index) override;
};

#endif  // FACT_REQUESTDATA_HPP

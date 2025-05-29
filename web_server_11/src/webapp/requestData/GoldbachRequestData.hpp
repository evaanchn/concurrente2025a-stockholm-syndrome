// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef GOLDBACH_REQUESTDATA_HPP
#define GOLDBACH_REQUESTDATA_HPP

#include <iostream>
#include <vector>

#include "GoldbachCalculator.hpp"
#include "ConcurrentApp.hpp"
#include "CalcRequestData.hpp"

/// @brief Base class to save data from a client request throughout the
/// concurrent production line
class GoldbachRequestData : public CalcRequestData {
 public:
  /// Constructor
  GoldbachRequestData(HttpRequest& httpRequest, ConcurrentApp* concurrentApp) 
  : CalcRequestData(httpRequest, concurrentApp) {
    this->appCalculator = new GoldbachCalculator();
  }
};

#endif  // GOLDBACH_REQUESTDATA_HPP

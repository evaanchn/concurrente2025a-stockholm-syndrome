// Copyright 2021,2024 Jeisson Hidalgo-Cespedes. ECCI-UCR. CC BY 4.0
// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef REQUESTUNIT_HPP
#define REQUESTUNIT_HPP

#include <iostream>
#include <vector>

#include "RequestData.hpp"

/// @brief Record to store the private data for a request data unit
struct RequestUnit {
  /// Pointer to the shared data for all units from the same request
  RequestData* requestData;
  /// Assigned index to store a result
  size_t resultIndex;

  /// @brief Default constructor
  RequestUnit() = default;
  /// Constructor
  RequestUnit(RequestData* requestData, size_t resultIndex)
  : requestData(requestData) 
  , resultIndex(resultIndex) {
  }
};

#endif  // REQUESTUNIT_HPP

// Copyright 2021,2024 Jeisson Hidalgo-Cespedes. ECCI-UCR. CC BY 4.0
// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef REQUESTUNIT_HPP
#define REQUESTUNIT_HPP

#include <iostream>
#include <vector>

class RequestData;

/// @brief Record to store the private data for a request data unit
struct RequestUnit {
 public:
  /// Pointer to the shared data for all units from the same request
  RequestData* requestData = nullptr;
  /// Assigned index to store a result
  size_t resultIndex = 0;

 public:
  /// @brief Default constructor
  explicit RequestUnit(RequestData* requestData = nullptr
      , size_t resultIndex = 0)
  : requestData(requestData)
  , resultIndex(resultIndex) {
  }
  /// Compare two RequestUnit objects for equality
  /// @return true if both objects attributes are equal, false otherwise
  inline bool operator==(const RequestUnit& other) const {
    return this->requestData == other.requestData
      && this->resultIndex == other.resultIndex;
  }
};

#endif  // REQUESTUNIT_HPP

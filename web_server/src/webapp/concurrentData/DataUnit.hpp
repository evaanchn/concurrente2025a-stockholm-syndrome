// Copyright 2021,2024 Jeisson Hidalgo-Cespedes. ECCI-UCR. CC BY 4.0
// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef REQDATAUNIT_HPP
#define REQDATAUNIT_HPP

#include <iostream>
#include <vector>

// forward declaration
class ConcurrentData;

/// @brief Record to store the private data for a data unit
struct DataUnit {
 public:
  /// Pointer to the shared data for all units
  ConcurrentData* concurrentData = nullptr;
  /// Assigned index to store a result
  size_t resultIndex = 0;

 public:
  /// @brief Default constructor
  explicit DataUnit(ConcurrentData* concurrentData = nullptr
      , size_t resultIndex = 0)
  : concurrentData(concurrentData)
  , resultIndex(resultIndex) {
  }
  /// Compare two DataUnit objects for equality
  /// @return true if both objects attributes are equal, false otherwise
  inline bool operator==(const DataUnit& other) const {
    return this->concurrentData == other.concurrentData
      && this->resultIndex == other.resultIndex;
  }
};

#endif  // DATAUNIT_HPP

// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef WORKERUNIT_HPP
#define WORKERUNIT_HPP

#include <iostream>
#include <vector>

#include "DataUnit.hpp"

// forward declaration
class ConcurrentData;

/// @brief Dataunit that encapsulates the information required by master server
/// when sending results back to the original request client.
struct WorkerUnit : public DataUnit{
 public:
    /// Pointer to the original concurrent data from master server
    ConcurrentData* originalConcurrentData = nullptr;
    /// Assigned index to store a result in the original concurrent data
    size_t originalResultIndex = 0;

 public:
  /// @brief Default constructor
  explicit WorkerUnit(ConcurrentData* concurrentData = nullptr,
    size_t resultIndex = 0, ConcurrentData* originalConcurrentData = nullptr,
    size_t originalResultIndex = 0): DataUnit(concurrentData, resultIndex),
    originalConcurrentData(originalConcurrentData),
    originalResultIndex(originalResultIndex) {
  }
  /// Compare two WorkUnit objects for equality
  inline bool operator==(const WorkerUnit& other) const {
    return this->concurrentData == other.concurrentData
      && this->resultIndex == other.resultIndex
      && this->originalConcurrentData == other.originalConcurrentData
      && this->originalResultIndex == other.originalResultIndex;
  }
};

#endif  // WORKERUNIT_HPP

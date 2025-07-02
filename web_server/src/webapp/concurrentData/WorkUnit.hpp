// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef WORKUNIT_HPP
#define WORKUNIT_HPP

#include <cstdint>
#include <iostream>
#include <vector>

#include "DataUnit.hpp"

// forward declaration
class ConcurrentData;

/// @brief Dataunit that encapsulates the information required by master server
/// when sending results back to the original request client.
struct WorkUnit : public DataUnit {
 public:
    /// Pointer to the original concurrent data from master server
    ConcurrentData* originalConcurrentData = nullptr;
    /// Assigned index to store a result in the original concurrent data
    size_t originalResultIndex = 0;

 public:
  /// @brief Default constructor
  explicit WorkUnit(ConcurrentData* concurrentData = nullptr,
    size_t resultIndex = 0, ConcurrentData* originalConcurrentData = nullptr,
    size_t originalResultIndex = 0): DataUnit(concurrentData, resultIndex),
    originalConcurrentData(originalConcurrentData),
    originalResultIndex(originalResultIndex) {
  }

 public:
  /// @brief Parse processed data into a string
  /// @return parsed text
  /// @note Used in worker process
  std::string serializeResponse() override {
    // Obtain original pointer to concurrent data
    uintptr_t originalDataPtr = reinterpret_cast<std::uintptr_t>
      (this->originalConcurrentData);

    std::stringstream responseData;  // Buffer for response's data
    responseData << this->concurrentData->getAppIndex() << '\n' <<
      originalDataPtr << '\n' <<
      this->originalResultIndex << '\n' <<
      this->concurrentData->serializeResult(this->resultIndex) << '\n';

    return responseData.str();
  }

  /// Compare two WorkUnit objects for equality
  inline bool operator==(const WorkUnit& other) const {
    return this->concurrentData == other.concurrentData
      && this->resultIndex == other.resultIndex
      && this->originalConcurrentData == other.originalConcurrentData
      && this->originalResultIndex == other.originalResultIndex;
  }
};

#endif  // WORKUNIT_HPP

// Copyright 2021,2024 Jeisson Hidalgo-Cespedes. ECCI-UCR. CC BY 4.0
// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef REQUESTUNIT_HPP
#define REQUESTUNIT_HPP

#include <iostream>
#include <vector>

#include "RequestData.hpp"

/// @brief Stores the private data for a request data unit from the query
/// @tparam InputType request data
/// @tparam OutputType results data
template <typename InputType, typename OutputType>
class RequestUnit {
 protected:
  /// User input to processed
  InputType data;
  /// PoInputTypeer to the shared data for all units from the same request
  RequestData<InputType, OutputType>* requestData;
  /// Assigned index to store a result
  size_t resultIndex;

 public:
  /// Constructor
  explicit RequestUnit(RequestData<InputType, OutputType>* requestData)
  : requestData(requestData) {
  }
  DISABLE_COPY(RequestUnit);
  /// Data unit to be processed
  /// @return const access to the data
  const InputType& getData() {
    return this->data;
  }
  /// Shared data from all request units
  /// @return pointer to the requestData
  RequestData<InputType, OutputType>* getRequestData() {
    return this->requestData;
  }
  /// Obtain index assigned inside the shared data from all units
  /// @return unsigned index value
  size_t getResultIndex() {
    return this->resultIndex;
  }
};

#endif  // REQUESTUNIT_HPP

// Copyright 2021,2024 Jeisson Hidalgo-Cespedes. ECCI-UCR. CC BY 4.0
// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef REQUESTDATA_HPP
#define REQUESTDATA_HPP

#include <iostream>
#include <vector>

#include "HttpApp.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "AppCalculator.hpp"

// TODO(us): delete when class is created
typedef HttpApp ConcurrentApp;

/// @brief Generic class to save data from a client request throughout the
/// concurrent production line
/// @tparam InputType request data
/// @tparam OutputType results data
template <typename InputType, typename OutputType>
class RequestData {
 protected:
  /// Store input units to be processed into a @RequestUnit
  std::vector<InputType> query;
  /// Control whether the request is ready to be answered or not
  size_t pendingQueries;
  /// Store results obtained from all request units
  std::vector<std::vector<OutputType>> results;
  /// Response containing the client socket to return the requested result
  HttpResponse httpResponse;
  /// Pointer to the assigned app to manage the request
  ConcurrentApp* concurrentApp;
  /// Pointer to the assigned calculator to process queries
  AppCalculator* calculator;

 public:
  /// Constructor
  RequestData(HttpRequest& httpRequest, ConcurrentApp* concurrentApp
  , AppCalculator* calculator)
  : pendingQueries(0)
  , httpResponse(httpRequest.getSocket(), httpRequest.getHttpVersion())
  , concurrentApp(concurrentApp)
  , calculator(calculator) {
  }
  DISABLE_COPY(RequestData);
  /// Destructor
  ~RequestData() {
  }
  /// Access to the corresponding httpResponse from the request
  HttpResponse& getHttpResponse() {
    return this->httpResponse;
  }
  /// Access to the corresponding app from the request
  ConcurrentApp* getConcurrentApp() {
    return this->concurrentApp;
  }
  /// Access to the corresponding calculator from the request
  AppCalculator* getCalculator() {
    return this->calculator;
  }
  /// Get the request query containing all units to processed
  /// @return input type vector with all data units
  std::vector<InputType>& getQuery() {
    return this->query;
  }
  /// Add a new data unit to the query
  /// @param dataUnit Input type data
  /// @return itself
  RequestData& operator+=(const InputType dataUnit) {
    this->query.push_back(dataUnit);
    ++this->pendingQueries;
    return *this;
  }
  /// Store a given result
  /// @param newResult list of data from a new result, must be at leat one
  /// @param index index inside the results vector, conditional safety
  void saveResult(std::vector<OutputType> newResult, size_t index) {
    if (!newResult.empty()) {
      // one less query to be conpleted
      if (!this->isReady()) this->results.at(index) = newResult;
    }
  }
  /// Indicate that an answered request unit was recieved and is ready to be
  /// part of the response
  /// @remark to be called only by a ResponseAssembler (single thread)
  void unitReady() {
    if (pendingQueries > 0) --pendingQueries;
  }
  /// Ask if all request units where completed and so the response can be
  /// created
  /// @return true if pendingQueries is 0, false otherwise
  bool isReady() {
    return (pendingQueries == 0);
  }
};

#endif  // REQUESTDATA_HPP

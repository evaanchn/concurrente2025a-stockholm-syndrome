// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef REQUESTDATA_HPP
#define REQUESTDATA_HPP

#include <iostream>
#include <vector>

#include "ConcurrentApp.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "RequestUnit.hpp"

/// @brief Base class to save data from a client request throughout the
/// concurrent production line
class RequestData {
 protected:
  /// Control whether the request is ready to be answered or not
  size_t pendingQueries;
  /// Response containing the client socket to return the requested result
  HttpResponse httpResponse;
  /// Pointer to the assigned app to manage the request
  ConcurrentApp* concurrentApp = nullptr;

 public:
  /// Constructor
  explicit RequestData(HttpRequest& httpRequest
      , ConcurrentApp* concurrentApp = nullptr);
  /// Destructor
  virtual ~RequestData() = default;
  /// Access to the corresponding httpResponse from the request
  HttpResponse& getHttpResponse();
  /// Access to the corresponding app from the request
  ConcurrentApp* getConcurrentApp();
  /// Decompose the queries into RequestUnits
  /// @return a vector of RequestUnits, each containing an index to save results
  virtual std::vector<RequestUnit> decompose() = 0;
  /// @brief Process a queries for the request
  /// @param index index to obtain queries
  virtual void processQuery(size_t index) = 0;
  /// Indicate that an answered request unit was recieved and is ready to be
  /// part of the response
  /// @remark to be called only by a ResponseAssembler (single thread)
  void signalUnitReady();
  /// Ask if all request units where completed and so the response can be
  /// created
  /// @return true if pendingQueries is 0, false otherwise
  bool isReady();
  /// Take the results and format them into the httpResponse, then send it
  virtual void respond() = 0;
};

#endif  // REQUESTDATA_HPP

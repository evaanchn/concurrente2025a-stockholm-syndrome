// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef REQUESTDATA_HPP
#define REQUESTDATA_HPP

// TODO(us) Add "concurrent" to the name

#include <iostream>
#include <vector>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
// #include "RequestUnit.hpp"

struct RequestUnit;

/// @brief Base class to save data from a client request throughout the
/// concurrent production line
class RequestData {
 protected:
  /// Control whether the request is ready to be answered or not
  size_t pendingQueries;
  /// Request containing the client request to be processed
  HttpRequest httpRequest;
  /// Response containing the client socket to return the requested result
  HttpResponse httpResponse;

 public:
  /// Constructor
  explicit RequestData(const HttpRequest& httpRequest
      , const HttpResponse& httpResponse);
  /// Destructor
  virtual ~RequestData() = default;
  /// Decompose the queries into RequestUnits
  /// @return a vector of RequestUnits, each containing an index to save results
  virtual std::vector<RequestUnit> decompose() = 0;
  /// @brief Process a queries for the request
  /// @param index index to obtain queries
  virtual void processQuery(size_t index) = 0;
  /// Indicate that an answered request unit was recieved and is ready to be
  /// part of the response
  /// @remark to be called only by a ResponseAssembler (single thread)
  void markUnitReady();
  /// Ask if all request units where completed and so the response can be
  /// created
  /// @return true if pendingQueries is 0, false otherwise
  bool isReady() const;
  /// Take the results and format them into the httpResponse, then send it
  virtual void respond() = 0;
};

#endif  // REQUESTDATA_HPP

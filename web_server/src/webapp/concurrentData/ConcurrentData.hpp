// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef CONCURRENTDATA_HPP
#define CONCURRENTDATA_HPP

// TODO(us) Add "concurrent" to the name

#include <iostream>
#include <vector>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
// #include "DataUnit.hpp"

struct DataUnit;

/// @brief Base class to save data from a client request throughout the
/// concurrent production line
class ConcurrentData {
 protected:
  /// Control whether data is ready to be answered or not
  size_t pendingQueries;
  /// Request containing the client request to be processed
  HttpRequest httpRequest;
  /// Response containing the client socket to return the requested result
  HttpResponse httpResponse;

 public:
  /// Constructor
  explicit ConcurrentData(const HttpRequest& httpRequest
      , const HttpResponse& httpResponse);
  /// Destructor
  virtual ~ConcurrentData() = default;
  /// Decompose the queries into ataUnits
  /// @return a vector of DataUnits, each containing an index to save results
  virtual std::vector<DataUnit> decompose() = 0;
  /// @brief Process a query
  /// @param index index to obtain queries
  virtual void processQuery(size_t index) = 0;
  /// Indicate that an answered data unit was recieved and is ready to be
  /// part of the response
  /// @remark to be called only by a ResponseAssembler (single thread)
  void markUnitReady();
  /// Ask if all data units where completed and so the response can be
  /// created
  /// @return true if pendingQueries is 0, false otherwise
  bool isReady() const;
  /// Take the results and format them into the httpResponse, then send it
  virtual void respond() = 0;
};

#endif  // CONCURRENTDATA_HPP

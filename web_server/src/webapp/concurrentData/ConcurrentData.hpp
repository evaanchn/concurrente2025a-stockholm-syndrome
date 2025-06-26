// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef CONCURRENTDATA_HPP
#define CONCURRENTDATA_HPP

#include <iostream>
#include <string>
#include <vector>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

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
  /// Index of the application that created this ConcurrentData
  size_t appIndex;

 public:
  /// Constructor
  explicit ConcurrentData(const HttpRequest& httpRequest,
    const HttpResponse& httpResponse, const size_t appIndex);
  /// Destructor
  virtual ~ConcurrentData() = default;
  /// Decompose the queries into ataUnits
  /// @return a vector of DataUnits, each containing an index to save results
  virtual std::vector<DataUnit*> decompose() = 0;
  /// @brief Get application index
  /// @remarks Caller must ensure index in not out of bounds
  size_t getAppIndex() const;
  /// @brief Serialize a query into a string
  virtual std::string serializeQuery(size_t queryID) const = 0;
  /// @brief Serialize the result of a query into a string
  virtual std::string serializeResult(size_t queryID) const = 0;
  /// @brief Deserialize a result from a string
  /// @throws std::runtime_error if the result is not valid
  virtual void deserializeResult(const size_t resultIndex,
    std::string& queryResult) = 0;
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

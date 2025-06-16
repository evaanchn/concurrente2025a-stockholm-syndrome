// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef CALC_REQUESTDATA_HPP
#define CALC_REQUESTDATA_HPP

#include <iostream>
#include <string>
#include <vector>

#include "RequestData.hpp"

/// @brief Base calculation class to save data from a client request throughout
// the concurrent production line
class CalcRequestData : public RequestData {
 protected:
  /// Store input units to be processed into a RequestUnit
  std::vector<int64_t> queries;
  /// Store results obtained from all request units
  std::vector<std::vector<int64_t>> results;

 public:
  /// Constructor
  CalcRequestData(HttpRequest& httpRequest, HttpResponse& httpResponse);
  DISABLE_COPY(CalcRequestData);
  // Destructor
  virtual ~CalcRequestData() = default;
  /// @brief Obtain list of values from the httpRequest URI
  /// @return a vector of strings with the values from the URI
  const std::vector<std::string> getURIValues();
  /// @brief obtain request queries
  std::vector<int64_t>& getQueries();
  /// Update pendingQueries according to the queries size and reserve space to
  /// store results
  /// @remark to be called after queries are set
  void updatePending();
  /// @brief Format the response with the results of the request
  /// @details This method is called by the web server to format the response
  /// with the results of the request. It builds the HTML response body
  /// with the results of the calculation.
  /// @remark The results vector contains a vector of int64_t for each
  /// calculation, where the first element is the original value and the
  /// rest of the elements are the results of the calculation.
  void formatResponse();
  /// @brief Unary calculation response for a given value to be implemented by
  // the subclases
  /// @param value the original calculation value
  /// @param result vector containing the result elements
  virtual void buildResult(int64_t value, std::vector<int64_t>& result) = 0;
  /// Decompose the queries into RequestUnits
  /// @return a vector of RequestUnits, each containing an index to save results
  std::vector<RequestUnit> decompose() override;
  /// Take the results and format them into the httpResponse, then send it
  void respond() override;
};

#endif  // CALC_REQUESTDATA_HPP

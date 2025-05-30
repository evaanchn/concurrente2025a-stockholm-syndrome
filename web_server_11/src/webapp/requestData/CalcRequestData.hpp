// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef CALC_REQUESTDATA_HPP
#define CALC_REQUESTDATA_HPP

#include <iostream>
#include <vector>

#include "ConcurrentApp.hpp"
#include "RequestData.hpp"
#include "RequestUnit.hpp"

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
  CalcRequestData(HttpRequest& httpRequest, ConcurrentApp* concurrentApp);
  DISABLE_COPY(CalcRequestData);
  // Destructor
  virtual ~CalcRequestData() = default;
  /// @brief obtain request queries
  std::vector<int64_t>& getQueries();
  /// Update pendingQueries according to the queries size and reserve space to
  /// store results
  /// @remark to be called after queries are set
  void updatePending();
  /// Decompose the queries into RequestUnits
  /// @return a vector of RequestUnits, each containing an index to save results
  std::vector<RequestUnit> decompose() override;
  /// Take the results and format them into the httpResponse, then send it
  void respond() override;
};

#endif  // CALC_REQUESTDATA_HPP

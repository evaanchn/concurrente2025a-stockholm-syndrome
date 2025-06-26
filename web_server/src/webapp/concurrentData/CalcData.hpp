// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef CALCDATA_HPP
#define CALCDATA_HPP

#include <iostream>
#include <string>
#include <vector>

#include "ConcurrentData.hpp"

/// @brief Base calculation class to save data from a client request throughout
// the concurrent production line
class CalcData : public ConcurrentData {
 protected:
  /// Store input units to be processed into a DataUnit
  std::vector<int64_t> queries;
  /// Store results obtained from all request units
  std::vector<std::vector<int64_t>> results;
  const std::string resultsPrefix = "=";

 public:
  /// Constructor
  CalcData(HttpRequest& httpRequest, HttpResponse& httpResponse,
    const size_t appIndex);
  DISABLE_COPY(CalcData);
  // Destructor
  virtual ~CalcData() = default;
  /// @brief Obtain list of values from the httpRequest URI
  /// @return a vector of strings with the values from the URI
  const std::vector<std::string> getURIValues();
  /// @brief Get the queries vector
  /// @details This method returns a reference to the queries vector, which
  /// contains the original values to be processed.
  std::vector<int64_t>& getQueries();
  /// Update pendingQueries according to the queries size and reserve space to
  /// store results
  /// @remark to be called after queries are set
  void updatePending();
  /// @brief Serialize a query into a string
  std::string serializeQuery(size_t queryIndex) const override;
  /// @brief Serialize the result of a query into a string
  std::string serializeResult(size_t queryIndex) const override;
  /// @brief Deserialize a result from a string
  /// @throws std::runtime_error if the result is not valid
  void deserializeResult(const size_t resultIndex
    , std::string& queryResult) override;
  /// @brief Format the response with the results of data
  /// @details This method is called by the web server to format the response
  /// with the results of a shared data. It builds the HTML response body
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
  /// Decompose the queries into data units
  /// @return a vector of DataUnit pointers
  /// results
  std::vector<DataUnit*> decompose() override;
  /// Take the results and format them into the httpResponse, then send it
  void respond() override;
};

#endif  // CALCDATA_HPP

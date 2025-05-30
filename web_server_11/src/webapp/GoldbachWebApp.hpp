// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef GOLDBACHWEBAPP_HPP
#define GOLDBACHWEBAPP_HPP

#include <string>
#include <vector>


#include "CalcWebApp.hpp"
#include "GoldbachCalculator.hpp"
#include "HttpApp.hpp"

/**
@brief A web application that calculates Goldbach sums
*/
class GoldbachWebApp : public CalcWebApp {
  /// Objects of this class cannot be copied
  DISABLE_COPY(GoldbachWebApp);

 public:
  /// Constructor
  GoldbachWebApp();
  /// Destructor
  ~GoldbachWebApp() = default;

 private:
  /// @brief Prime factorization for a given value
  /// @param result vector containing the original value and its sums
  /// @param httpResponse The object to answer to the client/user
  void buildResult(std::vector<int64_t>& result
      , HttpResponse& httpResponse) override;

  /// @brief List the possible goldbach sums of value
  /// @param result results of goldbach sum: {value, sum1val, sum1val, ...}
  /// @param httpResponse The object to answer to the client/user
  void sumsResponse(std::vector<int64_t>& result, HttpResponse& httpResponse);

    /// @brief Create a RequestData object to store the request data
  /// @param httpRequest is the request to be handled
  /// @return a pointer to the created RequestData object
  RequestData* createRequestData(HttpRequest& httpRequest) override;
};

#endif  // GOLDBACHWEBAPP_HPP

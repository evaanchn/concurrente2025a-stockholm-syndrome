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
  /// @param httpReponse The object to answer to the client/user
  void buildResult(std::vector<int64_t>& result
    , HttpResponse& httpResponse) override;

  /// @brief Count of possible goldbach sums of value
  /// @param sums results of goldbach sum: {value, sum1val, sum1val, ...}
  /// @param httpResponse The object to answer to the client/user
  void sumsCountResponse(int64_t sumsCount, HttpResponse& httpResponse);

  /// @brief List the possible goldbach sums of value
  /// @param sums results of goldbach sum: {value, sum1val, sum1val, ...}
  /// @param httpResponse The object to answer to the client/user
  void sumsResponse(std::vector<int64_t>& sums, HttpResponse& httpResponse);
};

#endif  // GOLDBACHWEBAPP_HPP

// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef GOLDBACHWEBAPP_HPP
#define GOLDBACHWEBAPP_HPP

#include <string>
#include <vector>

#include "HttpApp.hpp"
#include "CalcWebApp.hpp"

/**
@brief A web application that calculates Goldbach sums
*/
class GoldbachWebApp : public CalcWebApp<int64_t> {
  /// Objects of this class cannot be copied
  DISABLE_COPY(GoldbachWebApp);

 public:
  /// Constructor
  GoldbachWebApp();
  /// Destructor
  ~GoldbachWebApp() = default;

 private:
  /// @brief Goldbach sums for a given value
  /// @param value given number
  /// @param httpResponse The object to answer to the client/user
  void buildResponse(const int64_t value, HttpResponse& httpResponse) override;

  /// @brief Count of possible goldbach sums of value
  /// @param sums results of goldbach sum: {value, sum1val, sum1val, ...}
  /// @param httpResponse The object to answer to the client/user
  void sumsCountResponse(int64_t value, int64_t sumsCount
    , HttpResponse& httpResponse);

  /// @brief List the possible goldbach sums of value
  /// @param sums results of goldbach sum: {value, sum1val, sum1val, ...}
  /// @param httpResponse The object to answer to the client/user
  void sumsResponse(std::vector<int64_t>& sums, HttpResponse& httpResponse);
};

#endif  // GOLDBACHWEBAPP_HPP

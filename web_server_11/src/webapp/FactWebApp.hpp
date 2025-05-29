// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef FACTWEBAPP_HPP
#define FACTWEBAPP_HPP

#include <string>
#include <vector>

#include "FactRequestData.hpp"
#include "CalcWebApp.hpp"
#include "HttpApp.hpp"
#include "FactRequestData.hpp"

/**
@brief A web application that calculates prime factors
*/
class FactWebApp : public CalcWebApp {
  /// Objects of this class cannot be copied
  DISABLE_COPY(FactWebApp);

 public:
  /// Constructor
  FactWebApp();
  /// Destructor
  ~FactWebApp() = default;

 private:
  /// @brief Prime factorization for a given value
  /// @param result vector containing the original value and its prime factors
  /// @param httpReponse The object to answer to the client/user
  void buildResult(std::vector<int64_t>& result, HttpResponse& httpResponse);

  /// @brief Answer with the possible prime factors of a value
  /// @param primeFactors results of prime factorization: {count, fact1,
  /// exponent1, ...}
  /// @param value number to evaluate
  /// @param primeFactorsCount Number of factors in primeFactors
  /// @param httpResponse The object to answer to the client/user
  void factResponse(std::vector<int64_t>& primeFactors, const int64_t value
    , int64_t primeFactorsCount, HttpResponse& httpResponse);
  
  /// @brief Create a RequestData object to store the request data
  /// @param httpRequest is the request to be handled
  /// @param httpResponse is the response to be sent back to the client
  /// @return a pointer to the created RequestData object
  RequestData* createRequestData(HttpRequest& httpRequest) override;
};

#endif  // FACTWEBAPP_HPP
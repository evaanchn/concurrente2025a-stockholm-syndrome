// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef FACTWEBAPP_HPP
#define FACTWEBAPP_HPP

#include <string>
#include <vector>

#include "CalcWebApp.hpp"
#include "HttpApp.hpp"
#include "PrimeFactCalculator.hpp"

/**
@brief A web application that calculates prime factors
*/
class FactWebApp : public CalcWebApp<int64_t> {
  /// Objects of this class cannot be copied
  DISABLE_COPY(FactWebApp);

 public:
  /// Constructor
  FactWebApp();
  /// Destructor
  ~FactWebApp() = default;
  
 private:
  /// @brief Prime factorization for a given value
  /// @param value given number
  /// @param httpReponse The object to answer to the client/user
  void buildResponse(const int64_t value, HttpResponse& httpResponse) override;

  /// @brief Answer with the possible prime factors of a value
  /// @param sums results of prime factorization: {count, fact1, exponent1, ...}
  /// @param httpResponse The object to answer to the client/user
  void factResponse(std::vector<int64_t>& facts, HttpResponse& httpResponse); 
};

#endif  // FACTWEBAPP_HPP

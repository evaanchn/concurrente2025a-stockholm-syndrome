// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef FACTWEBAPP_HPP
#define FACTWEBAPP_HPP

#include <string>
#include <vector>

#include "CalcWebApp.hpp"
#include "FactRequestData.hpp"
#include "HttpApp.hpp"

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
  /// @brief Create a RequestData object to store the request data
  /// @param httpRequest is the request to be handled
  /// @return a pointer to the created RequestData object
  RequestData* createRequestData(HttpRequest& httpRequest) override;
};

#endif  // FACTWEBAPP_HPP

// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef FACTWEBAPP_HPP
#define FACTWEBAPP_HPP

#include "CalcWebApp.hpp"

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

 public:
  /// @brief Create a ConcurrentData object to store the shared data
  /// @param httpRequest is the request to be handled
  /// @param httpResponse is the response to be handled
  /// @param appIndex index of the application that created this data
  /// @return a pointer to the created ConcurrentData object
  ConcurrentData* createConcurrentData(HttpRequest& httpRequest
      , HttpResponse& httpResponse, const size_t appIndex) override;
};

#endif  // FACTWEBAPP_HPP

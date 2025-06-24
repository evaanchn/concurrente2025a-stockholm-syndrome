// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef GOLDBACHWEBAPP_HPP
#define GOLDBACHWEBAPP_HPP

#include <string>

#include "CalcWebApp.hpp"

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
  /// @brief Create a ConcurrentData object to store data
  /// @param httpRequest is the request to be handled
  /// @param httpResponse is the response to be handled
  /// @return a pointer to the created ConcurrentData object
  ConcurrentData* createConcurrentData(HttpRequest& httpRequest
    , HttpResponse& httpResponse) override;
};

#endif  // GOLDBACHWEBAPP_HPP

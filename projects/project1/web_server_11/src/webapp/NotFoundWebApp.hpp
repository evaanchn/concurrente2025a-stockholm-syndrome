// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#ifndef NOTFOUNDWEBAPP_HPP
#define NOTFOUNDWEBAPP_HPP

#include <string>
#include <vector>

#include "HttpApp.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

/// @brief App prefixes
static const char* const FACT_PREFIX = "/fact";
static const char* const GOLDBACH_PREFIX = "/goldbach";

/**
 @brief A home web application to access calculator web apps
 */
class NotFoundWebApp : public HttpApp {
 public:
  /// Objects of this class cannot be copied
  DISABLE_COPY(NotFoundWebApp);
  /// Constructor
  NotFoundWebApp();
  /// Destructor
  ~NotFoundWebApp();
  /// Called by the web server when the web server is started
  void start() override;
  /// @brief pendiente
  /// @param httpRequest 
  /// @param httpResponse 
  /// @return 
  static bool serveNotFound(HttpRequest& httpRequest
    , HttpResponse& httpResponse);
  /// Called when the web server stops, in order to allow the web application
  /// clean up and finish as well
  void stop() override;
};

#endif
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
  /// Sends a page for a non found resource in this server. This method is
  /// called if none of the registered web applications handled the request.
  /// If you want to override this method, create a web app, e.g NotFoundWebApp
  /// that reacts to all URIs, and chain it as the last web app
  static bool serveNotFound(HttpRequest& httpRequest
    , HttpResponse& httpResponse);
  /// Called when the web server stops, in order to allow the web application
  /// clean up and finish as well
  void stop() override;
};

#endif

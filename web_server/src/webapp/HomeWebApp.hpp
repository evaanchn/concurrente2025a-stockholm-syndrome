// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef HOMEWEBAPP_HPP
#define HOMEWEBAPP_HPP

#include <string>
#include <vector>

#include "HttpApp.hpp"

/// @brief App prefixes
static const char* const FACT_PREFIX = "/fact";
static const char* const GOLDBACH_PREFIX = "/goldbach";

/**
 @brief A home web application to access calculator web apps
 */
class HomeWebApp : public HttpApp {
 public:
  /// Objects of this class cannot be copied
  DISABLE_COPY(HomeWebApp);
  /// Constructor
  HomeWebApp();
  /// Destructor
  ~HomeWebApp();
  /// Called by the web server when the web server is started
  void start() override;
  /// Handle HTTP requests. @see HttpServer::handleHttpRequest()
  /// @return true If this application handled the request, false otherwise
  /// and another chained application should handle it
  bool handleHttpRequest(HttpRequest& httpRequest
    , HttpResponse& httpResponse) override;
  /// @brief Utility method to serve the common header for each page
  static void serveHeader(HttpResponse& httpResponse, const std::string& title);
  /// Called when the web server stops, in order to allow the web application
  /// clean up and finish as well
  void stop() override;

 private:
  /// Handle HTTP requests. @see HttpServer::handleHttpRequest()
  /// @return true If this application handled the request, false otherwise
  /// Sends the homepage as HTTP response
  bool serveHomepage(HttpRequest& httpRequest, HttpResponse& httpResponse);
};

#endif

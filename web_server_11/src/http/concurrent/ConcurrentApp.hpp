// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
#ifndef CONCURRENTAPP_HPP
#define CONCURRENTAPP_HPP

#include <vector>
#include <iostream>

#include "HttpApp.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "RequestData.hpp"

/// @brief ConcurrentApp is a base class for web applications that handle
/// @details This class is designed
/// to handle HTTP requests in a concurrent environment, allowing for
/// efficient processing of multiple requests simultaneously.
/// It provides methods to check if the application can handle a request
/// and to parse the request, extracting numbers of socket.
class ConcurrentApp : public HttpApp {
 public:
  /// Constructor
  ConcurrentApp() = default;
  /// Objects of this class cannot be copied
  DISABLE_COPY(ConcurrentApp);
  ~ConcurrentApp() = default;
  /// @brief Handle HTTP requests.
  /// @details This method is called by the web server to handle HTTP requests.
  /// It checks if the application can handle the request and sets the
  /// response status code to 307 (Temporary Redirect) if it can.
  /// @param httpRequest is the request to be handled
  /// @param httpResponse is the response to be sent back to the client
  /// @return true if this application can handle the request, false otherwise
  bool handleHttpRequest(HttpRequest& httpRequest,
      HttpResponse& httpResponse) override {
    (void)httpResponse;  // parameter is not required
    if (this->canHandleHttpRequest(httpRequest)) {
      return true;
    }
    return false;
  }

  /// @brief Check if this application can handle the HTTP request
  /// @param httpRequest request to be handled
  /// @return true if this application can handle the request,
  virtual bool canHandleHttpRequest(HttpRequest& httpRequest) = 0;
  /// @brief Parse the HTTP request to parse numbers from the URI
  virtual RequestData* createRequestData(HttpRequest& httpRequest) = 0;
};

#endif  // CONCURRENTAPP_HPP

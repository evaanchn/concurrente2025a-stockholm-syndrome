// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
#ifndef CoNCURRENTAPP_HPP
#define CoNCURRENTAPP_HPP

#include <vector>
#include <iostream>

#include "HttpApp.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

/// @brief Generic class to handle concurrent HTTP requests
/// @details This class is designed
/// to handle HTTP requests in a concurrent environment, allowing for
/// efficient processing of multiple requests simultaneously.
/// It provides methods to check if the application can handle a request
/// and to parse the request, extracting numbers of socket.
/// @tparam InputType request data
/// @tparam OutputType results data
template <typename InputType, typename OutputType>
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
    if (this->canHandleHttpRequest(httpRequest, httpResponse)) {
      httpResponse.setStatusCode(307);
      return true;
    }
    return false;
  }

  /// @brief Check if this application can handle the HTTP request
  /// @param httpRequest request to be handled
  /// @param httpResponse response to be sent back to the client
  /// @return true if this application can handle the request,
  virtual bool canHandleHttpRequest(HttpRequest& httpRequest,
      HttpResponse& httpResponse) = 0;

  /// @brief Parse the HTTP request to parse data units from the URI
  /// @param httpRequest request to be parsed
  /// @param httpResponse response to the client
  /// @param query is the vector to store the parsed data
  virtual void parseRequest(HttpRequest& httpRequest, HttpResponse& httpResponse
      , std::vector<InputType>& query) = 0;

  /// @brief Format the response with the results of the request
  /// @param results is the vector of results to be formatted
  /// @param httpResponse is the response to be sent back to the client
  virtual void formatResponse(std::vector<std::vector<OutputType>>& results,
      HttpResponse& httpResponse) = 0;
};


#endif  // CoNCURRENTAPP_HPP

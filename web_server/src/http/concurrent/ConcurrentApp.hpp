// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
#ifndef CONCURRENTAPP_HPP
#define CONCURRENTAPP_HPP

#include <string>

#include "HttpApp.hpp"

class WorkUnit;

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
  ConcurrentData* createConcurrentData(HttpRequest& httpRequest
      , HttpResponse& httpResponse, const size_t appIndex) override = 0;

  /// @brief Create a plain text from DataUnit to be send into the network
  /// @note Used in master process
  std::string serializeRequest(DataUnit* dataUnit) override = 0;
  /// @brief Create a work unit from the serialized data sent from master
  /// @note Used in worker process
  WorkUnit* deserializeRequest(std::string requestData) override = 0;

  /// @brief Create a work unit from the serialized data sent from master
  /// @return parsed text
  /// @note Used in worker process
  std::string serializeResponse(WorkUnit*) override = 0;
  /// @brief Parse received text into the response to a DataUnit
  /// @note Used in master process
  DataUnit* deserializeResponse(std::string responseData) override = 0;
};

#endif  // CONCURRENTAPP_HPP

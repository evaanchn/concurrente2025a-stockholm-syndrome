// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#ifndef HTTPAPP_H
#define HTTPAPP_H

#include <string>

#include "common.hpp"

// forward declarations
class HttpRequest;
class HttpResponse;
class ConcurrentData;
struct DataUnit;

/**
@brief Base class for all web applications that can be registered with the
web server.
*/
class HttpApp {
  /// Web application objects are usually complex. This base class does not
  /// require child classes to allow copying
  DISABLE_COPY(HttpApp);

 public:
  /// Constructor
  HttpApp() = default;
  /// Destructor
  ~HttpApp() = default;
  /// Called by the web server when the web server is started
  virtual void start();
  /// Handle HTTP requests. @see HttpServer::handleHttpRequest()
  /// @return true If this application handled the request, false otherwise
  /// and another chained application should handle it
  virtual bool handleHttpRequest(HttpRequest& httpRequest
      , HttpResponse& httpResponse) = 0;
  /// Called when the web server stops, in order to allow the web application
  /// clean up and finish as well
  virtual void stop();
  /// @brief Parse the HTTP request and create request data.
  /// @return nullptr by default
  virtual ConcurrentData* createConcurrentData(HttpRequest& httpRequest
    , HttpResponse& httpResponse, const size_t appIndex);
  /// @brief Create a plain text from DataUnit to be send into the network
  /// @return empty text by default
  virtual std::string serializeRequest(DataUnit* dataUnit);
  /// @brief Parse received text into the response to a DataUnit
  /// @return nullptr by default
  virtual DataUnit* deserializeResponse(std::string responseData);
  /// @brief Parse received text into the response to a WorkerUnit
  /// @return nullptr by default
  virtual DataUnit* deserializeRequest(std::string requestData);
};

#endif  // HTTPAPP_H

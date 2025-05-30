// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#include <string>

#include "HttpConnectionHandler.hpp"

HttpConnectionHandler::HttpConnectionHandler
  (std::vector<HttpApp*>& applications)
  : applications(applications) {}

int HttpConnectionHandler::run() {
  // Start consuming sockets from queue and enqueue request data if
  // request is from concurrent app
  this->consumeLoop();
  // Produce a stop condition for consumer of producing queue, after stopping
  this->produce(nullptr);
  return EXIT_SUCCESS;
}

void HttpConnectionHandler::consume(Socket clientConnection) {
  // While the same client asks for HTTP requests in the same connection
  while (true) {
    // Create an object that parses the HTTP request from the socket
    HttpRequest httpRequest(clientConnection);

    // If the request is not valid or an error happened
    if (!httpRequest.parse()) {
      // Non-valid requests are normal after a previous valid request. Do not
      // close the connection yet, because the valid request may take time to
      // be processed. Just stop waiting for more requests
      break;
    }

    // A complete HTTP client request was received. Create an object for the
    // server responds to that client's request
    HttpResponse httpResponse(clientConnection, httpRequest.getHttpVersion());

    // Give subclass a chance to respond the HTTP request
    const bool handled = this->handleHttpRequest(httpRequest, httpResponse);

    // If subclass did not handle the request or the client used HTTP/1.0
    if (!handled || httpRequest.getHttpVersion() == "HTTP/1.0") {
      // The socket will not be used anymore, thus we close the connection
      clientConnection.close();
      break;
    }
  }
}

bool HttpConnectionHandler::handleHttpRequest(HttpRequest& httpRequest,
HttpResponse& httpResponse) {
  // Print IP and port from client
  const NetworkAddress& address = httpRequest.getNetworkAddress();
  Log::append(Log::INFO, "connection",
    std::string("connection established with client ") + address.getIP()
    + " port " + std::to_string(address.getPort()));

  // Print HTTP request
  Log::append(Log::INFO, "request", httpRequest.getMethod()
    + ' ' + httpRequest.getURI()
    + ' ' + httpRequest.getHttpVersion());

  return this->route(httpRequest, httpResponse);
}

bool HttpConnectionHandler::route(HttpRequest& httpRequest
    , HttpResponse& httpResponse) {
  // Traverse the chain of applications
  for (size_t index = 0; index < this->applications.size(); ++index) {
    // If this application handles the request
    HttpApp* app = this->applications[index];
    if (app->handleHttpRequest(httpRequest, httpResponse)) {
      // If handleHttpRequest returned true, it could be a concurrent app
      ConcurrentApp* concurrentApp = dynamic_cast<ConcurrentApp*>(app);
      // If it is concurrent (dynamic cast successful)
      if (concurrentApp) {
        // Create request data
        RequestData* request = concurrentApp->createRequestData(httpRequest
            , httpResponse);
        // Ensure request was created successfully
        if (request) {
          // Enqueue request data to be processed by the concurrent app
          this->produce(request);
        }
      }
      return true;
    }
  }
  // NotFoundWebApp should take care of requests that don't belong to any app
  // Thus this should never happen, so the assert(false) alerts the programmer
  assert(false);
  return false;
}

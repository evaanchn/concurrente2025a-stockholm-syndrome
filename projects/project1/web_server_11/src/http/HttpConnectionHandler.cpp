// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#include <string>

#include "HttpConnectionHandler.hpp"

HttpConnectionHandler::HttpConnectionHandler
  (std::vector<HttpApp*>& applications)
  : applications(applications) {}

int HttpConnectionHandler::run() {
  // start consumming sockets
  this->consumeLoop();
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
  for (size_t index = 0; index < this->applications.size() - 1; ++index) {
    // If this application handles the request
    HttpApp* app = this->applications[index];
    if (app->handleHttpRequest(httpRequest, httpResponse)) {
      return true;
    }
  }

  // Unrecognized request, must be handled by NotFoundWebApp
  // NotFoundWebApp at applications vector last element
  return this->applications.back()->handleHttpRequest(httpRequest
    , httpResponse);
}

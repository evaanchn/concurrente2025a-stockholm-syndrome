// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#ifndef HTTPCONNECTIONHANDLER_HPP
#define HTTPCONNECTIONHANDLER_HPP

#include <vector>

#include "Assembler.hpp"
#include "Socket.hpp"
#include "Log.hpp"
#include "NetworkAddress.hpp"
#include "HttpApp.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "ConcurrentApp.hpp"
#include "RequestData.hpp"

/**
 * @class HttpConnectionHandler
 * @brief Thread object that manages a connection with a client (one socket)
 */
class HttpConnectionHandler : public Assembler<Socket, RequestData*> {
 private:
  /// Reference to app chain in server
  std::vector<HttpApp*>& applications;

 public:
  DISABLE_COPY(HttpConnectionHandler);

  /// Constructor
  explicit HttpConnectionHandler(std::vector<HttpApp*>& applications);

  /// @brief Start consuming loop
  /// @return EXIT SUCCESS / EXIT FAILURE
  int run() override;

  /// @brief "Consume" socket by attending user requests
  /// @param clientConnection: Connection with client
  void consume(Socket clientConnection) override;

 private:
  /// @brief Handles a client's request
  /// @param httpRequest The client request
  /// @param httpResponse The response object to prepare
  /// @return true if the request was handled correctly
  bool handleHttpRequest(HttpRequest& httpRequest,
    HttpResponse& httpResponse);

  /// Called each time an HTTP request is received. HttpConnectionHandler should
  ///  analyze the request object and assemble a response with the response
  /// object. Finally send the response calling the httpResponse.send() method.
  /// @param httpRequest The request object to analyze
  /// @param httpResponse The response object to prepare
  /// @return true if the request was handled correctly
  bool route(HttpRequest& httpRequest, HttpResponse& httpResponse);
};

#endif  // HTTPCONNECTIONHANDLER_HPP

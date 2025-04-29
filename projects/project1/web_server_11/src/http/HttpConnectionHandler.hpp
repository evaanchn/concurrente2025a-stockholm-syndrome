// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#ifndef HTTPCONNECTIONHANDLER_HPP
#define HTTPCONNECTIONHANDLER_HPP

#include <vector>

#include "Consumer.hpp"
#include "Socket.hpp"
#include "Log.hpp"
#include "NetworkAddress.hpp"
#include "HttpApp.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class HttpConnectionHandler : public Consumer<Socket> {
 public:
  DISABLE_COPY(HttpConnectionHandler);
  explicit HttpConnectionHandler(std::vector<HttpApp*>& applications);

  int run() override;

  /// @brief "Consume" socket by attending user requests
  /// @param clientConnection: Connection with client
  void consume(Socket clientConnection) override;

 private:
  /// Reference to app chain in server
  std::vector<HttpApp*>& applications;

 private:
  /// @brief Handles a client's request
  /// @param httpRequest The client request
  /// @param httpResponse The response object to prepare
  /// @return true if the request was handled correctly
  virtual bool handleHttpRequest(HttpRequest& httpRequest,
    HttpResponse& httpResponse);

  /// @brief Asks each app to handle the request
  /// @see handleHttpRequest
  bool route(HttpRequest& httpRequest, HttpResponse& httpResponse);
};

#endif  // HTTPCONNECTIONHANDLER_HPP

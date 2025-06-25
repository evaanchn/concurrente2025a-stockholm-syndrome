// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include "HttpApp.hpp"
#include "MasterServer.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Log.hpp"
#include "NetworkAddress.hpp"
#include "Socket.hpp"

const char* const usage =
  "Usage: webserv [port] [max_connections] [queue_capacity]\n"
  "\n"
  "  port        Network port to listen incoming HTTP requests, default "
    DEFAULT_PORT "\n"
  "  max_connections  Max amount of connections the server"
  " can attend concurrently\n"
  "  conn_queue_capacity  Max amount of connections that"
  " can be stored in queue\n";

MasterServer::MasterServer() {
}

MasterServer::~MasterServer() {
}

MasterServer& MasterServer::getInstance() {
  static MasterServer server;
  return server;
}

int MasterServer::run() {
  // Start the server and listen for connections
  try {
    this->listenForConnections(this->port);
    this->acceptAllConnections();
  }catch (const std::runtime_error& error) {
    std::cerr << "Error: " << error.what() << std::endl;
  }
  return EXIT_SUCCESS;
}


void MasterServer::handleClientConnection(Socket& workerConnection) {
  // Handle the client connection by sending it to the worker connections
  std::string password;
  if (workerConnection >> password && password == WORKER_PASSWORD) {
      workerConnections.addConnection(workerConnection);
      this->produce(workerConnection);
  }
}

void MasterServer::stop() {
  // Stop listening for incoming client connection requests
  this->stopListening();
  for (size_t i = 0; i < this->stopConditionToSend; ++i) {
    this->produce(Socket());  // Send stop condition to handlers
  }
}

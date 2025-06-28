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
#include "WorkerConnections.hpp"

MasterServer::MasterServer(WorkerConnections& workerConnections,
      size_t stopConditionsToSend):
  stopConditionsToSend(stopConditionsToSend),
  workerConnections(workerConnections) {
}

MasterServer::~MasterServer() {
}

int MasterServer::run() {
  // Start the server and listen for connections
  try {
    this->listenForConnections(this->port);
    this->acceptAllConnections();
  } catch (const std::runtime_error& error) {
    std::cerr << "Error: " << error.what() << std::endl;
    this->stop();
  }
  return EXIT_SUCCESS;
}

void MasterServer::listenForever(const char* port) {
  return TcpServer::listenForever(port);
}

void MasterServer::handleClientConnection(Socket& workerConnection) {
  // Handle the client connection by sending it to the worker connections
  std::string password;
  if (workerConnection.readLine(password, '\n')) {
    if (password == WORKER_PASSWORD) {
      workerConnections.addConnection(workerConnection);
      this->produce(workerConnection);
    }
  }
}

void MasterServer::stop() {
  // Stop listening for incoming client connection requests
  this->stopListening();
  for (size_t i = 0; i < this->stopConditionsToSend; ++i) {
    this->produce(Socket());  // Send stop condition to handlers
  }
}

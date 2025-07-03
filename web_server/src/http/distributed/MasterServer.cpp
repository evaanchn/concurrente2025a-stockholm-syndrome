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

// Initialize with worker connections queue and stop conditions count
MasterServer::MasterServer(WorkerConnections& workerConnections,
      size_t stopConditionsToSend):
  stopConditionsToSend(stopConditionsToSend),
  workerConnections(workerConnections) {
}

// Empty destructor as cleanup is handled elsewhere
MasterServer::~MasterServer() {
}

// Main server execution loop
int MasterServer::run() {
  // Start the server and listen for connections
  try {
    // Bind to port and start listening for incoming connections
    this->listenForConnections(this->port);
    // Get network information for logging
    const NetworkAddress& address = this->getNetworkAddress();
    Log::append(Log::INFO, "master", "Available at " + address.getIP()
      + " port " + std::to_string(address.getPort()));
    // Main server loop - accepts connections until stopped
    // Each connection spawns a new thread handled by handleClientConnection()
    this->acceptAllConnections();
  } catch (const std::runtime_error& error) {
    // Critical error handling - log to stderr and initiate shutdown
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
      // Add authenticated connection to worker pool
      workerConnections.addConnection(workerConnection);
      // Queue connection for processing by worker threads
      this->produce(workerConnection);
    }
  }
}

void MasterServer::stop() {
  // Send stop condition to handlers
  for (size_t i = 0; i < this->stopConditionsToSend; ++i) {
    this->produce(Socket());
  }
}

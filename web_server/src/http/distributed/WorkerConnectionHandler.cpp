// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
#include <string>
#include "HttpApp.hpp"

#include "Log.hpp"
#include "NetworkAddress.hpp"
#include "WorkerConnectionHandler.hpp"

// Initialize with reference to applications and worker connections pool
WorkerConnectionHandler::WorkerConnectionHandler
  (std::vector<HttpApp*>& applications,
   WorkerConnections& workerConnections) :
    applications(applications),
    workerConnections(workerConnections) {
}

// Main execution loop for the handler thread
int WorkerConnectionHandler::run() {
  // Start consuming sockets from queue and enqueue request data if
  // request is from concurrent app
  this->consumeLoop();
  // Produce a stop condition for consumer of producing queue, after stopping
  this->produce(nullptr);
  return EXIT_SUCCESS;
}

// Processes a single worker connection until it's closed
void WorkerConnectionHandler::consume(Socket workerConnection) {
  // Print IP and port from worker
  const NetworkAddress& address = workerConnection.getNetworkAddress();
  Log::append(Log::INFO, "connection",
    std::string("connection established with worker ") + address.getIP()
    + " port " + std::to_string(address.getPort()));

  // While the same client asks for HTTP requests in the same connection
  while (true) {
    // If the workerConnection is not connected, stop consuming
    if (!this->route(workerConnection)) {
      // Clean up connection from worker pool before breaking
      this->workerConnections.removeSocket(workerConnection);
      break;
    }
  }
}

// Handles request routing for a single worker connection
bool WorkerConnectionHandler::route(Socket& workerConnection) {
  // First read which application should handle this request
  std::string appIndexStr;
  if (!workerConnection.readLine(appIndexStr, '\n')) {
    return false;  // error reading line
  }
  // Validate application index
  size_t appIndex = 0;
  try {
    appIndex = std::stoul(appIndexStr);
    if (appIndex >= this->applications.size()) {
      throw std::invalid_argument("appIndex out of range");
    }
  } catch(const std::invalid_argument& e) {
    Log::append(Log::ERROR, "worker", "Invalid app index: " + appIndexStr);
    return false;  // invalid app index
  }
  // Read fixed number of lines that make up the response data
  std::string buffer;
  for (size_t i = 0; i < RESPONSE_BUFFER_LINES_COUNT; ++i) {
    std::string line;
    if (!workerConnection.readLine(line, '\n')) {
      Log::append(Log::ERROR, "worker", "Error reading request line");
      return false;  // error reading line
    }
    buffer += line + '\n';
  }
  // Delegate response processing to the appropriate application
  DataUnit* dataUnit = this->applications[appIndex]->
    deserializeResponse(buffer);
  if (dataUnit == nullptr) {
    return false;  // error deserializing response
  }
  // Queue the processed data unit for further handling
  this->produce(dataUnit);
  return true;  // successfully routed request
}

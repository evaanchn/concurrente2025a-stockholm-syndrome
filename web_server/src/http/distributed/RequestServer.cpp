// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include <string>
#include <vector>

#include "RequestServer.hpp"
#include "ConcurrentApp.hpp"
#include "DataUnit.hpp"
#include "HttpApp.hpp"
#include "Log.hpp"

RequestServer::RequestServer
  (std::vector<HttpApp*>& applications, Socket& masterConnection,
    size_t stopConditionsToSend) :
    applications(applications),
    masterConnection(masterConnection),
    stopConditionsToSend(stopConditionsToSend) {
}

int RequestServer::run() {
  // Start consuming from the masterConnection connection until it is closed
  this->handleMasterConnection();

  // Before stopping, send the amount of stop conditions needed for the
  // calculators
  for (size_t i = 0; i < this->stopConditionsToSend; ++i) {
    this->produce(nullptr);  // Stop condition of producing queue
  }

  return EXIT_SUCCESS;
}

void RequestServer::handleMasterConnection() {
  while (true) {
    // Read the data from the masterConnection connection
    DataUnit* work = this->readRequestFromMaster();
    if (work == nullptr) {
      // If there was an error reading the request, stop consuming
      Log::append(Log::ERROR, "worker", "Error reading request from master");
      break;
    }
    // Produce the WorkUnit from received request
    this->produce(work);
  }
}

DataUnit* RequestServer::readRequestFromMaster() {
  std::string buffer;
  // Read the application index from the masterConnection connection
  if (!(this->masterConnection.readLine(buffer, '\n'))) {
    return nullptr;
  }
  size_t appIndex = 0;
  try {
    // Convert the received string to unsigned long for app index
    appIndex = std::stoul(buffer);
    // Validate the application index is within bounds
    if (appIndex >= this->applications.size()) {
      throw std::invalid_argument("appIndex out of range");
    }
  } catch(const std::invalid_argument& e) {
    // Log error if index is invalid (non-numeric or out of range)
    Log::append(Log::ERROR, "worker", "Invalid app index: " + buffer);
    return nullptr;
  }

  // Read the request lines from the masterConnection
  buffer += '\n';  // appIndex separator
  std::string line;
  for (size_t i = 0; i < REQUEST_BUFFER_LINES_COUNT; ++i) {
    if (!this->masterConnection.readLine(line, '\n')) {
      // If any line fails to read, log error and abort
      Log::append(Log::ERROR, "worker", "Error reading request line");
      return nullptr;  // error reading line
    }
    // Append each line to the buffer with newline separator
    buffer += line + '\n';
  }
  // Deserialize the request into a WorkUnit
  DataUnit* work = this->applications[appIndex]->deserializeRequest(buffer);
  return work;
}

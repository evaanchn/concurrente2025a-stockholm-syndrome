// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
#include "WorkerConnectionHandler.hpp"
#include <string>
#include "HttpApp.hpp"

WorkerConnectionHandler::WorkerConnectionHandler
  (std::vector<HttpApp*>& applications)
  : applications(applications) {
}


int WorkerConnectionHandler::run() {
  // Start consuming sockets from queue and enqueue request data if
  // request is from concurrent app
  this->consumeLoop();
  // Produce a stop condition for consumer of producing queue, after stopping
  this->produce(nullptr);
  return EXIT_SUCCESS;
}

void WorkerConnectionHandler::consume(Socket workerConnection) {
  // While the same client asks for HTTP requests in the same connection
  while (true) {
    // If the password is not valid, set free to receive a new connection
    if (!route(workerConnection)) {
      break;
    }
  }
}

bool WorkerConnectionHandler::route(Socket& workerConnection) {
  std::string appIndexStr;
  if (!workerConnection.readLine(appIndexStr, '\n')) {
    return false;  // error reading line
  }
  size_t appIndex = 0;
  try {
    appIndex = std::stoul(appIndexStr);
  } catch(const std::invalid_argument& e) {
    Log::append(Log::ERROR, "worker", "Invalid app index: " + appIndexStr);
    return false;  // invalid app index
  }

  std::string buffer;
  for (size_t i = 0; i < RESPONSE_BUFFER_LINES_COUNT; ++i) {
    if (!workerConnection.readLine(buffer, '\n')) {
      Log::append(Log::ERROR, "worker", "Error reading request line");
      return false;  // error reading line
    }
  }
  DataUnit* dataUnit = this->applications[appIndex]->
    deserializeResponse(buffer);
  if (dataUnit == nullptr) {
    return false;  // error deserializing response
  }
  this->produce(dataUnit);
  return true;  // successfully routed request
}

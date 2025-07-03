// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include <string>
#include <vector>

#include "common.hpp"
#include "ResponseClient.hpp"
#include "ConcurrentData.hpp"
#include "HttpApp.hpp"
#include "WorkUnit.hpp"

#include "Log.hpp"

ResponseClient::ResponseClient(size_t pendingStopConditions) :
  pendingStopConditions(pendingStopConditions) {
}

Socket& ResponseClient::connect(const char* server, const char* port) {
  this->TcpClient::connect(server, port);
  // give access password on success
  this->socket << WORKER_PASSWORD;
  this->socket.send();
  return this->getSocket();
}

int ResponseClient::run() {
  // std::string ip = "10.84.145.21";
  // std::string port = DEFAULT_MASTER_PORT;
  // this->TcpClient::connect(ip.c_str(), port.c_str());
  // Start consuming from own queue, until receiving necessary amount of stops
  for (size_t i = 0; i < this->pendingStopConditions; ++i) {
    this->consumeLoop();
  }
  // TODO: send stop condition to master server
  this->socket.close();
  return EXIT_SUCCESS;
}

void ResponseClient::consume(DataUnit* unit) {
  assert(unit);
  // serialize the result
  std::string queryResult = unit->serializeResponse();
  // Send the result to the master server
  this->socket << queryResult;
  // Send the result index to the master server
  if (this->socket.send()) {
    // If the result was sent successfully, we can delete the unit
    Log::append(Log::INFO, "worker", "Result sent successfully");
  } else {
    Log::append(Log::ERROR, "worker", "Error sending result to master server");
    
  }

  delete unit->concurrentData;  // Delete the ConcurrentData object
  delete unit;  // Unit is not used anymore
}

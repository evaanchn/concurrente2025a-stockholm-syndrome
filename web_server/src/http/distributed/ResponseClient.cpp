// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "ResponseClient.hpp"
#include "ConcurrentData.hpp"
#include "HttpApp.hpp"
#include "WorkUnit.hpp"

ResponseClient::ResponseClient(std::vector<HttpApp*>& applications,
  size_t pendingStopConditions): applications(applications),
    pendingStopConditions(pendingStopConditions) {
}

Socket& ResponseClient::connect(const char* server, const char* port) {
  this->TcpClient::connect(server, port);
}

int ResponseClient::run() {
  // std::string ip = "10.84.145.21";
  // std::string port = DEFAULT_MASTER_PORT;
  // this->TcpClient::connect(ip.c_str(), port.c_str());
  this->socket << WORKER_PASSWORD;
  this->socket.send();
  // Start consuming from own queue, until receiving necessary amount of stops
  for (size_t i = 0; i < this->pendingStopConditions; ++i) {
    this->consumeLoop();
  }
  return EXIT_SUCCESS;
}

void ResponseClient::consume(DataUnit* unit) {
  assert(unit);
  // serialize the result
  size_t appIndex = unit->concurrentData->getAppIndex();
  std::string queryResult = this->applications[appIndex]->
    serializeResponse(unit);
  // Send the result to the master server
  if (this->socket << queryResult) {
    this->socket.send();
  }
  delete unit->concurrentData;  // Delete the ConcurrentData object
  delete unit;  // Unit is not used anymore
}
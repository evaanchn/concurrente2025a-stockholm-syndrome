// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include <string>

#include "RequestClient.hpp"
#include "WorkerConnections.hpp"
#include "ConcurrentData.hpp"
#include "DataUnit.hpp"

RequestClient::RequestClient(WorkerConnections& workerConnections,
  std::vector<HttpApp*>& applications):
    workerConnections(workerConnections),
    applications(applications) {
}

int RequestClient::run() {
  // Start consuming data units from queue
  this->consumeLoop();
  // Send stop condition back to Distributor
  this->produce(nullptr);
}

void RequestClient::consume(DataUnit* unit) {
  // TODO(any): uncomment for concurrent data and serialization modifications
  try {
    ConcurrentData* concurrentData = unit->concurrentData;
    size_t appIndex = 0;   // concurrentData->getAppIndex();
    std::string serializedUnit = ""; // this->applications[appIndex].serializeRequest(unit);

    // Get random connection to send stuff first
    Socket& connection = this->workerConnections.getRandomWorkerConnection();
    connection << serializedUnit;  // Add serialized data to socket

    // Try to send message to worker
    if (!connection.send()) {
      throw std::runtime_error("could not send request");
    }
    delete unit;  // Free memory
  } catch (const std::exception& e) {
    // Report error
    std::cerr << "Request client error: " << e.what() << std::endl;
    // Send unit back to Distributor
    this->produce(unit);
  }
}

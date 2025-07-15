// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include <string>
#include <vector>

#include "Distributor.hpp"
#include "WorkerConnections.hpp"
#include "ConcurrentData.hpp"
#include "DataUnit.hpp"
#include "HttpApp.hpp"

Distributor::Distributor(std::vector<HttpApp*>& applications,
    WorkerConnections& workerConnections, size_t stopConditionsToSend):
  applications(applications),
  workerConnections(workerConnections),
  stopConditionsToSend(stopConditionsToSend) {
}

int Distributor::run() {
  // Start consuming data units from queue
  this->consumeLoop();
  // Stop all workers, and remove the connections
  this->workerConnections.stopWorkers();
  // Send stop conditions to calculators of the non-distributed queue
  for (size_t i = 0; i < this->stopConditionsToSend; ++i) {
    this->produce(nullptr);
  }

  return EXIT_SUCCESS;
}


void Distributor::consume(DataUnit* unit) {
  try {
    // Get random connection to send stuff first
    Socket connection = this->workerConnections.getRandomWorkerConnection();
    // Serialize the data unit to send
    ConcurrentData* concurrentData = unit->concurrentData;
    size_t appIndex = concurrentData->getAppIndex();
    std::string serializedUnit =
      this->applications[appIndex]->serializeRequest(unit);
    connection << serializedUnit;  // Add serialized data to socket
    // Try to send message to worker
    if (!connection.send()) {
      throw std::runtime_error("could not send request");
    }
    delete unit;  // Free memory
  } catch (const std::exception& e) {
    // Send unit to local calculators
    this->produce(unit);
  }
}

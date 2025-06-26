// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "Distributor.hpp"
#include "WorkerConnections.hpp"

Distributor::Distributor(WorkerConnections& workerConnections,
      size_t stopConditionsToSend):
  workerConnections(workerConnections),
  stopConditionsToSend(stopConditionsToSend) {
}

int Distributor::run() {
  // Dispatch the data units according to existence of connected workers
  this->consumeLoop();

  // First receive a stop condition from Decomposer
  // Signal to RequestClient to stop
  this->toQueues[DISTRIBUTED]->enqueue(nullptr);

  // Wait for potential send-backs and eventual stop condition
  this->consumeLoop();

  // Send stop conditions to consumers of the non-distributed queue
  for (size_t i = 0; i < this->stopConditionsToSend; ++i) {
    this->toQueues[!DISTRIBUTED]->enqueue(nullptr);
  }

  return EXIT_SUCCESS;
}

bool Distributor::extractKey(DataUnit* const&) const {
  // Key determines if data is sent to RequestClient (workers connected,
  // distribution possible), or to own data unit processors (none connected)
  return this->workerConnections.hasConnections();
}

// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "ClientResponder.hpp"
#include "ConcurrentData.hpp"

ClientResponder::ClientResponder() {
}

int ClientResponder::run() {
  // Consume from own queue
  this->consumeLoop();

  return EXIT_SUCCESS;
}

void ClientResponder::consume(ConcurrentData* concurrentData) {
  assert(concurrentData);
  // format the response and send it to the client
  concurrentData->respond();
  // delete the concurrent data since It is no longer needed
  delete concurrentData;
}

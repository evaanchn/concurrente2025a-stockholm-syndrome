// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "ClientResponder.hpp"

ClientResponder::ClientResponder(size_t pendingStopConditions) 
    : pendingStopConditions(pendingStopConditions) {
}

int ClientResponder::run() {
  //
  this->consumeLoop();
  return EXIT_SUCCESS;
}

void ClientResponder::consume(RequestData* requestData) {
  assert(requestData);
  // 
  requestData->respond();  
  delete requestData;
}

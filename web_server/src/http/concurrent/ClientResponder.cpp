// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "ClientResponder.hpp"

ClientResponder::ClientResponder() {
}

int ClientResponder::run() {
  // Consume from own queue
  this->consumeLoop();

  return EXIT_SUCCESS;
}

void ClientResponder::consume(RequestData* requestData) {
  assert(requestData);
  // format the response and send it to the client
  requestData->respond();
  // delete the request data since It is no longer needed
  delete requestData;
}

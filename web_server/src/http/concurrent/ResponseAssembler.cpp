// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "ResponseAssembler.hpp"

ResponseAssembler::ResponseAssembler(size_t pendingStopConditions)
    : pendingStopConditions(pendingStopConditions) {
}

int ResponseAssembler::run() {
  // Start consuming from own queue, until receiving necessary amount of stops
  for (size_t i = 0; i < this->pendingStopConditions; ++i) {
    this->consumeLoop();
  }

  // Produce stop condition for client responder
  this->produce(nullptr);

  return EXIT_SUCCESS;
}

void ResponseAssembler::consume(RequestUnit unit) {
  RequestData* requestData = unit.requestData;
  requestData->markUnitReady();
  // Check if data for response is complete, if so, enqueue
  if (requestData->isReady()) {
    this->produce(requestData);
  }
}

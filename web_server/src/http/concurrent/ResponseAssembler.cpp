// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "ResponseAssembler.hpp"

ResponseAssembler::ResponseAssembler(size_t pendingStopConditions
    , size_t stopConditionsToSend)
    : pendingStopConditions(pendingStopConditions)
    , stopConditionsToSend(stopConditionsToSend) {
}

int ResponseAssembler::run() {
  // Start consuming from own queue
  while (this->pendingStopConditions > 0) {
    this->consumeLoop();
    --this->pendingStopConditions;
  }
  // Produce stop conditions for the client responders
  for (size_t i = 0; i < this->stopConditionsToSend; ++i) {
    this->produce(nullptr);
  }
  return EXIT_SUCCESS;
}

void ResponseAssembler::consume(RequestUnit unit) {
  RequestData* requestData = unit.requestData;
  requestData->signalUnitReady();
  // Check if data for response is complete, if so, enqueue
  if (requestData->isReady()) {
    this->produce(requestData);
  }
}

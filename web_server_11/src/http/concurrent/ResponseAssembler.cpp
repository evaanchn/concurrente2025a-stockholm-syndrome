// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "ResponseAssembler.hpp"

ResponseAssembler::ResponseAssembler(size_t pendingStopConditions
    , size_t stopConditionsToSend) 
    : pendingStopConditions(pendingStopConditions)
    , stopConditionsToSend(stopConditionsToSend){
}

int ResponseAssembler::run() {
  this->consumeLoop();
  return EXIT_SUCCESS;
}

void ResponseAssembler::consume(RequestUnit unit) {
  RequestData* requestData = unit.requestData;
  requestData->signalUnitReady();
  if (requestData->isReady()) {
    this->produce(requestData);
  }
}

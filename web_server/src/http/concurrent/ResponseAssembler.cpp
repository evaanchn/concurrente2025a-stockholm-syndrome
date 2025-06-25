// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "ResponseAssembler.hpp"
#include "ConcurrentData.hpp"

ResponseAssembler::ResponseAssembler(size_t pendingStopConditions):
    pendingStopConditions(pendingStopConditions) {
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

void ResponseAssembler::consume(DataUnit* unit) {
  ConcurrentData* concurrentData = unit->concurrentData;
  concurrentData->markUnitReady();
  // Check if data for response is complete, if so, enqueue
  if (concurrentData->isReady()) {
    this->produce(concurrentData);
  }
  delete unit;  // Unit is not used anymore
}

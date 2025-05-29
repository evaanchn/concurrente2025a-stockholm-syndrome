// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
#include "Calculator.hpp"

int Calculator::run() {
  // Start consuming from the decomposer queue
  this->consumeLoop();
  return EXIT_SUCCESS;
}

void Calculator::consume(RequestUnit *unit) {
  // Get the request data
  RequestData* requestData = unit->requestData;
  // Process the number using the AppCalculator
  requestData->processQuery(unit->resultIndex);
  this->produce(unit);
}

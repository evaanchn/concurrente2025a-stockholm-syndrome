// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
#include "Calculator.hpp"
#include "RequestData.hpp"

int Calculator::run() {
  // Start consuming from the request units queue
  this->consumeLoop();
  this->produce(RequestUnit());  // Send stop condition to signal next consumer
  return EXIT_SUCCESS;
}

void Calculator::consume(RequestUnit unit) {
  // Get the request data
  RequestData* requestData = unit.requestData;
  // Process the number using the AppCalculator
  requestData->processQuery(unit.resultIndex);
  this->produce(unit);
}

// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
#include "Calculator.hpp"
#include "ConcurrentData.hpp"

int Calculator::run() {
  // Start consuming from the data units queue
  this->consumeLoop();
  this->produce(DataUnit());  // Send stop condition to signal next consumer
  return EXIT_SUCCESS;
}

void Calculator::consume(DataUnit unit) {
  // Get data
  ConcurrentData* concurrentData = unit.concurrentData;
  // Process the number using the AppCalculator
  concurrentData->processQuery(unit.resultIndex);
  this->produce(unit);
}

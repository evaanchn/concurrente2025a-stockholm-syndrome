// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
#include "Calculator.hpp"

int Calculator::run() {
  // Start consuming from the decomposer queue
  this->consumeLoop();
  return EXIT_SUCCESS;
}
void Calculator::consume(RequestUnit<int64_t, int64_t> *unit) {
  // Get the request data
  RequestData<int64_t, int64_t>* requestData = unit->getRequestData();
  // Get the input data
  int64_t number = unit->getData();
  std::vector<int64_t> result;
  // getCalculator() returns a pointer to the AppCalculator
  AppCalculator* calculator = requestData->getCalculator();
  // Process the number using the calculator
  calculator->processNumber(number, result);
  requestData->saveResult(result, unit->getResultIndex());
  this->produce(unit);
}

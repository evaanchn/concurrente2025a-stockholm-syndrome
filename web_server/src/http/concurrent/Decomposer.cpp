// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include <vector>

#include "Decomposer.hpp"
#include "ConcurrentData.hpp"

Decomposer::Decomposer(size_t pendingStopConditions):
    pendingStopConditions(pendingStopConditions) {}

int Decomposer::run() {
  // Consume from decomposer queue until having received pendingStopConditions
  // amounts of stop conditions.
  for (size_t i = 0; i < this->pendingStopConditions; ++i) {
    this->consumeLoop();
  }
  // Before stopping, send the amount of stop conditions needed for the workers
  // in the producing queue
  this->produce(nullptr);  // Stop condition of producing queue

  return EXIT_SUCCESS;
}

void Decomposer::consume(ConcurrentData* concurrentData) {
  // Decompose data into units
  std::vector<DataUnit*> units = concurrentData->decompose();
  // Enqueue each unit
  for (auto* unit : units) {
    this->produce(unit);
  }
}

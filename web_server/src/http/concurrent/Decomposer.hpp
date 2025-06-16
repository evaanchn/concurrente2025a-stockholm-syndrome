// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef DECOMPOSER_HPP
#define DECOMPOSER_HPP

#include <cstdlib>
#include <vector>

#include "Assembler.hpp"
#include "RequestData.hpp"
#include "RequestUnit.hpp"

/// @brief Decomposes a RequestData into RequestUnits
class Decomposer : public Assembler<RequestData*, RequestUnit> {
  DISABLE_COPY(Decomposer);

 private:
  /// @brief Equivalent to number of producers that produce to its queue
  /// Once all of them stop, decomposer stops too
  size_t pendingStopConditions = 0;
  /// @brief Amount of consumers of the queue decomposer produces to.
  /// must signal to each one that it has stopped for them to stop.
  size_t stopConditionsToSend = 0;

 public:
 /**
  * @brief Constructor
  * @param pendingStopConditions Amount of stop conditions decomposer must
  * consume to stop
  * @param stopConditionsToSend Amount of stop conditions next entity needs
  */
  Decomposer(size_t pendingStopConditions, size_t stopConditionsToSend);

  /// @brief Main procedure Decomposer thread will run
  /// @return EXIT_SUCCESS on return
  int run() override;

  /// @brief Consuming procedure
  /// @param data Request data pointer
  void consume(RequestData* data) override;
};

#endif  // DECOMPOSER_HPP

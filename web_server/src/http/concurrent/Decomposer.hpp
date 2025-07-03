// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef DECOMPOSER_HPP
#define DECOMPOSER_HPP

#include "Assembler.hpp"
#include "DataUnit.hpp"

// forward declaration
class ConcurrentData;

/// @brief Decomposes a ConcurrentData into data units
class Decomposer : public Assembler<ConcurrentData*, DataUnit*> {
  DISABLE_COPY(Decomposer);

 private:
  /// @brief Equivalent to number of producers that produce to its queue
  /// Once all of them stop, decomposer stops too
  size_t pendingStopConditions = 0;

 public:
 /**
  * @brief Constructor
  * @param pendingStopConditions Amount of stop conditions decomposer must
  * consume to stop
  */
  explicit Decomposer(size_t pendingStopConditions);

  /// @brief Main procedure Decomposer thread will run
  /// @return EXIT_SUCCESS on return
  int run() override;

  /// @brief Consuming procedure
  /// @param data shared data pointer
  void consume(ConcurrentData* data) override;
};

#endif  // DECOMPOSER_HPP

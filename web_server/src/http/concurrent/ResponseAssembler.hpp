// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef RESPONSEASSEMBLER_HPP
#define RESPONSEASSEMBLER_HPP

#include <cstdlib>

#include "Assembler.hpp"
#include "DataUnit.hpp"

// forward declaration
class ConcurrentData;

/// @brief
class ResponseAssembler : public Assembler<DataUnit, ConcurrentData*> {
  DISABLE_COPY(ResponseAssembler);

 private:
  /// Amount of stop conds needed from data units handlers
  /// to stop operating (no data units left to assemble and send)
  size_t pendingStopConditions = 0;

 public:
  /// @brief Constructor of the class
  explicit ResponseAssembler(size_t pendingStopConditions);

  /// @brief Destructor of the class
  ~ResponseAssembler() = default;

  /// @brief starts to consume all completed data units
  int run() override;

  /// @brief Consuming procedure
  /// @param unit Concurrent data unit
  void consume(DataUnit unit) override;
};

#endif  // RESPONSEASSEMBLER_HPP

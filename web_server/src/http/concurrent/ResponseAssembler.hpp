// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef RESPONSEASSEMBLER_HPP
#define RESPONSEASSEMBLER_HPP

#include <cstdlib>

#include "Assembler.hpp"
#include "RequestUnit.hpp"

// forward declaration
class RequestData;

/// @brief
class ResponseAssembler : public Assembler<RequestUnit, RequestData*> {
  DISABLE_COPY(ResponseAssembler);

 private:
  /// Amount of stop conds needed from request units handlers
  /// to stop operating (no request units left to assemble and send)
  size_t pendingStopConditions = 0;

 public:
  /// @brief Constructor of the class
  explicit ResponseAssembler(size_t pendingStopConditions);

  /// @brief Destructor of the class
  ~ResponseAssembler() = default;

  /// @brief starts to consume all the pending request
  int run() override;

  /// @brief Consuming procedure
  /// @param unit Request unit
  void consume(RequestUnit unit) override;
};

#endif  // RESPONSEASSEMBLER_HPP

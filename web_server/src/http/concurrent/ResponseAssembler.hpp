// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef RESPONSEASSEMBLER_HPP
#define RESPONSEASSEMBLER_HPP

#include <cstdlib>

#include "Assembler.hpp"
#include "RequestData.hpp"

/// @brief
class ResponseAssembler : public Assembler<RequestUnit, RequestData*> {
  DISABLE_COPY(ResponseAssembler);

 private:
  size_t pendingStopConditions = 0;
  size_t stopConditionsToSend = 0;

 public:
  /// @brief Constructor of the class
  ResponseAssembler(size_t pendingStopConditions,
      size_t stopConditionsToSend = 1);

  /// @brief Destructor of the class
  ~ResponseAssembler() = default;

  /// @brief starts to consume all the pending request
  int run() override;

  /// @brief Consuming procedure
  /// @param unit Request unit
  void consume(RequestUnit unit) override;
};

#endif  // RESPONSEASSEMBLER_HPP

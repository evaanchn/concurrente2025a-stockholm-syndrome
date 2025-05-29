// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef RESPONSEASSEMBLER_HPP
#define RESPONSEASSEMBLER_HPP

#include <cstdlib>

#include "Assembler.hpp"
#include "RequestData.hpp"

/// @brief ResponseAssembler is a class that assembles responses from
/// RequestUnits and sends them back to the client.
/// @remarks Responsible of deleting the RequestData after processing it
class ClientResponder : public Consumer<RequestData*> {
  DISABLE_COPY(ClientResponder);

private:
  /// @brief Number of pending stop conditions to consume
  size_t pendingStopConditions = 0;

 public:
  /// @brief Constructor of the class
  explicit ClientResponder(size_t pendingStopConditions = 1);

  /// @brief Destructor of the class
  ~ClientResponder() = default;

  /// @brief starts to consume all the pending requests
  int run() override;

  /// @brief Consumes a RequestData and sends the response back to the client
  /// then, it deletes the RequestData object since it is no longer needed.
  /// @param data Pointer to the RequestData to be processed
  void consume(RequestData* data) override;
};

#endif  // RESPONSEASSEMBLER_HPP
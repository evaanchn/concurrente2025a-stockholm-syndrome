// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef CLIENTRESPONDER_HPP
#define CLIENTRESPONDER_HPP

#include "Assembler.hpp"

// forward declaration
class ConcurrentData;

/// @brief ResponseAssembler is a class that assembles responses from
/// ConcurrentData and sends them back to the client.
/// @remarks Responsible of deleting the ConcurrentData after processing it
class ClientResponder : public Consumer<ConcurrentData*> {
  DISABLE_COPY(ClientResponder);

 public:
  /// @brief Constructor of the class
  ClientResponder();

  /// @brief Destructor of the class
  ~ClientResponder() = default;

  /// @brief starts to consume all concurrent data completed
  int run() override;

  /// @brief Consumes a ConcurrentData and sends the response back to the client
  /// then, it deletes the ConcurrentData object since it is no longer needed.
  /// @param data Pointer to the ConcurrentData to be processed
  void consume(ConcurrentData* data) override;
};

#endif  // CLIENTRESPONDER_HPP

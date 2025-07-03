// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef DISTRIBUTOR_HPP
#define DISTRIBUTOR_HPP


#include "common.hpp"
#include "Dispatcher.hpp"

// Forward declarations
class WorkerConnections;
struct DataUnit;

/// @brief Redicrects data units to worker connections or calculators.
class Distributor : public Dispatcher<bool, DataUnit*> {
  DISABLE_COPY(Distributor);

 private:
  // Reference to container with worker connections
  WorkerConnections& workerConnections;
  // Amount of threads consuming from Master's own data unit queue
  size_t stopConditionsToSend = 0;

 public:
  /// @brief Constructor
  /// @param workerConnections Reference to worker connections collection
  /// @param stopConditionsToSend Amount of stop conditions next entity needs
  Distributor(WorkerConnections& workerConnections,
      size_t stopConditionsToSend);
  /// Start redirecting network messages
  int run() override;
  /// This method extracts the key from a data stored in the fromQueue
  bool extractKey(DataUnit* const& data) const override;
};

#endif  // DISTRIBUTOR_HPP

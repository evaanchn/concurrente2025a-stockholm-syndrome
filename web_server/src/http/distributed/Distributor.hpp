// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef DISTRIBUTOR_HPP
#define DISTRIBUTOR_HPP

#define DISTRIBUTED true

#include "Dispatcher.hpp"

// Forward declarations
class DataUnit;
class WorkerConnections;

class Distributor : public Dispatcher<bool, DataUnit*> {
  DISABLE_COPY(Distributor);

 private:
  // Reference to container with worker connections
  WorkerConnections& workerConnections;
  // Amount of threads consuming from Master's own data unit queue
  size_t stopConditionsToSend = 0;

 public:
  Distributor(WorkerConnections& workerConnections,
      size_t stopConditionsToSend);
  /// Start redirecting network messages
  int run() override;
  /// This method extracts the key from a data stored in the fromQueue
  bool extractKey(DataUnit* const& data) const override;
};

#endif  // DISTRIBUTOR_HPP

// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef DISTRIBUTOR_HPP
#define DISTRIBUTOR_HPP

#include <vector>

#include "common.hpp"
#include "Assembler.hpp"

// Forward declarations
class HttpApp;
class WorkerConnections;
struct DataUnit;

/// @brief Redirects data units to worker connections or calculators.
class Distributor : public Assembler<DataUnit*, DataUnit*> {
  DISABLE_COPY(Distributor);

 private:
  /// Reference to app chain in server
  std::vector<HttpApp*>& applications;
  // Reference to container with worker connections
  WorkerConnections& workerConnections;
  // Amount of threads consuming from Master's own data unit queue
  size_t stopConditionsToSend = 0;

 public:
  /// @brief Constructor
  /// @param applications Reference to server apps
  /// @param workerConnections Reference to worker connections collection
  /// @param stopConditionsToSend Amount of stop conditions next entity needs
  Distributor(std::vector<HttpApp*>& applications,
      WorkerConnections& workerConnections, size_t stopConditionsToSend);
  /// Start redirecting network messages
  int run() override;
  /// @brief Overriden consume projects
  /// @param unit Unit consumed
  void consume(DataUnit* unit) override;
};

#endif  // DISTRIBUTOR_HPP

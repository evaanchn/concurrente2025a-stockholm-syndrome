// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef REQUESTCLIENT_HPP
#define REQUESTCLIENT_HPP

#include <vector>

#include "Assembler.hpp"

// Forward declarations
class HttpApp;
class WorkerConnections;
struct DataUnit;

/// @brief Consumes DataUnit objects and sends them to the worker connections
class RequestClient : public Assembler<DataUnit*, DataUnit*> {
  DISABLE_COPY(RequestClient);

 private:
  /// Reference to app chain in server
  std::vector<HttpApp*>& applications;
  // Reference to container with worker connections
  WorkerConnections& workerConnections;

 public:
  /// @brief Constructor
  /// @param workerConnections Reference to worker connections collection
  /// @param applications Reference to apps
  RequestClient(WorkerConnections& workerConnections,
      std::vector<HttpApp*>& applications);

  /// @brief Loop consumption
  /// @return Success or faiure
  int run() override;

  /// @brief Overriden consume projects
  /// @param unit Unit consumed
  void consume(DataUnit* unit) override;
};

#endif  // REQUESTCLIENT_HPP

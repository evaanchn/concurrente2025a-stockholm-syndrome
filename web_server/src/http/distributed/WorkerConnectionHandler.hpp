// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
#ifndef WORKERCONNECTIONHANDLER_HPP
#define WORKERCONNECTIONHANDLER_HPP


#include <vector>
#include <string>

#include "common.hpp"
#include "Assembler.hpp"
#include "Log.hpp"
#include "Socket.hpp"
#include "WorkerConnections.hpp"

// forward declaration
class HttpApp;
struct DataUnit;

/**
 * @brief Thread object that manages a connection with a worker (one socket)
 * and reads work results from it to deserialize them into DataUnit objects.
 */
class WorkerConnectionHandler : public Assembler <Socket, DataUnit*> {
 private:
  /// Reference to app chain in server
  std::vector<HttpApp*>& applications;
  /// Reference to worker connections momitor
  WorkerConnections& workerConnections;

 public:
  DISABLE_COPY(WorkerConnectionHandler);

  /// Constructor
  explicit WorkerConnectionHandler(std::vector<HttpApp*>& applications,
    WorkerConnections& workerConnections);

  /// @brief Start consuming loop
  /// @return EXIT SUCCESS
  int run() override;

  /// @brief "Consume" socket by attending worker requests
  /// @param workerConnection comunication socket with the worker
  void consume(Socket workerConnection) override;

  /// @brief Routes the worker request to the appropriate application
  /// @param workerConnection The connection with the worker
  /// @return true if the request was routed successfully, false otherwise
  bool route(Socket& workerConnection);
};

#endif  // WORKERCONNECTIONHANDLER_HPP

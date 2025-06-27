// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
#ifndef WORKERCONNECTIONHANDLER_HPP
#define WORKERCONNECTIONHANDLER_HPP


#include <vector>

#include "common.hpp"
#include "Assembler.hpp"
#include "Log.hpp"
#include "Socket.hpp"
#include "string.h"

// forward declaration
class HttpApp;
class DataUnit;

/**
 * @class WorkerConnectionHandler
 * @brief Thread object that manages a connection with a worker (one socket)
 * and attends to worker requests that can be sent.
 */
class WorkerConnectionHandler : public Assembler <Socket, DataUnit*> {
  /// Reference to app chain in server
 private:
  std::vector<HttpApp*>& applications;

 public:
  DISABLE_COPY(WorkerConnectionHandler);

  /// Constructor
  explicit WorkerConnectionHandler(std::vector<HttpApp*>& applications);

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

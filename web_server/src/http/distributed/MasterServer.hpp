// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef MASTERSERVER_H
#define MASTERSERVER_H

#include <climits>
#include <csignal>
#include <thread>
#include <vector>

#include "DataUnit.hpp"
#include "Queue.hpp"
#include "TcpServer.hpp"
#include "WorkerConnectionHander.hpp"

#define DEFAULT_PORT "9090"
#define WORKER_PASSWORD "wokerSecret"

// forward declarations
class HttpApp;
class WorkerConnections;

class MasterServer : public TcpServer, public Producer<Socket> {
  DISABLE_COPY(MasterServer);

 protected:
  const char* port = DEFAULT_PORT;

 private:
  size_t stopConditionToSend = 0;
  WorkerConnections & workerConnections;

 public:
  /// Destructor
  ~MasterServer();
  // Unique (singleton pattern) instance of the server
  static MasterServer& getInstance();
  /// @brief Start the server and listen for incoming connections.
  /// @return EXIT_SUCCESS
  int run();
  /// @brief This method is called by the controller when it receives a
  /// signal to stop the server.
  void stop();

 protected:
  /// Constructor (not public for singleton pattern)
  MasterServer();
  /// @brief Handle a worker connection request.
  /// @param workerConnection comunication socket with the worker
  void handleClientConnection(Socket& workerConnection) override;
};

#endif  // MASTERSERVER_H

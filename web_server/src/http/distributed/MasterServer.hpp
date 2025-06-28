// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef MASTERSERVER_H
#define MASTERSERVER_H

#include <climits>
#include <csignal>
#include <thread>
#include <vector>

#include "common.hpp"
#include "DataUnit.hpp"
#include "Queue.hpp"
#include "TcpServer.hpp"
#include "WorkerConnectionHandler.hpp"
#include "WorkerConnections.hpp"
#include "Socket.hpp"

#define WORKER_PASSWORD "wokerSecret"

// forward declarations
class HttpApp;

class MasterServer : public TcpServer, public Producer<Socket> {
  DISABLE_COPY(MasterServer);

 protected:
  const char* port = DEFAULT_MASTER_PORT;

 private:
  size_t stopConditionsToSend = 0;
  WorkerConnections& workerConnections;

 public:
  /// Constructor (not public for singleton pattern)
  MasterServer(WorkerConnections& workerConnections,
      size_t stopConditionsToSend);
  /// Destructor
  ~MasterServer();
  /// @brief Start the server and listen for incoming connections.
  /// @return EXIT_SUCCESS
  int run() override;
  /// @brief Listen for incoming connections on the specified port.
  void listenForever(const char* port);
  /// @brief Stop the server from accepting new connections.
  void stop();

 protected:
  /// @brief Handle a worker connection request.
  /// @param workerConnection comunication socket with the worker
  void handleClientConnection(Socket& workerConnection) override;
};

#endif  // MASTERSERVER_H

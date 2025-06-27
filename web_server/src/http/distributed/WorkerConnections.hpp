// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef WORKERCONNECTIONS_HPP
#define WORKERCONNECTIONS_HPP

#include <mutex>
#include <random>
#include <vector>

#include "common.hpp"
#include "Socket.hpp"

// TODO(Andrey): make documentation
// Thread save monitor to control worker sockets
class WorkerConnections {
  DISABLE_COPY(WorkerConnections);

 private:
  std::vector<Socket> connections;
  std::mutex canAccessWorkerConnections;

 public:
  WorkerConnections() = default;
  ~WorkerConnections();
  Socket getRandomWorkerConnection();
  void removeSocket(int index);
  void addConnection(Socket& socket);
  void stopWorkers();
  bool hasConnections() const;
  size_t getConnectionCount() const;
};

#endif

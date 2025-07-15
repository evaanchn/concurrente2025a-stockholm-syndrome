// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef WORKERCONNECTIONS_HPP
#define WORKERCONNECTIONS_HPP

#include <mutex>
#include <vector>

#include "common.hpp"
#include "Socket.hpp"
#include "Util.hpp"

/**
 * @class WorkerConnections
 * @brief Thread-safe monitor for managing worker socket connections
 * 
 * Provides synchronized access to a pool of worker sockets with atomic operations
 * for connection management. Implements the monitor pattern using mutex locking
 * to ensure thread safety.
 */
class WorkerConnections {
  DISABLE_COPY(WorkerConnections);

 private:
  /// Pool of active worker socket connections
  std::vector<Socket> connections;
  /// Mutex for synchronizing access to connections
  std::mutex canAccessWorkerConnections;

 public:
  /**
   * @brief Constructs a new WorkerConnections instance
   */
  WorkerConnections() = default;
  /**
   * @brief Destroys the WorkerConnections and cleans up resources
   */
  ~WorkerConnections();
  /**
   * @brief Retrieves a random worker connection from the pool
   * @return Socket A connected worker socket
   * @note This is a thread-safe operation
   */
  Socket getRandomWorkerConnection();
  /**
   * @brief Removes a socket from the connection pool
   * @param socket The socket connection to remove
   * @note This is a thread-safe operation
   */
  void removeSocket(Socket& socket);
  /**
   * @brief Adds a new connection to the worker pool
   * @param socket The socket connection to add (passed by reference)
   * @note This is a thread-safe operation
   */
  void addConnection(Socket& socket);
  /**
   * @brief Stops all workers and clears the connection pool
   * @note This is a thread-safe operation
   */
  void stopWorkers();
  /**
   * @brief Gets the current number of active connections
   * @return size_t Count of connections in the pool
   * @note This is a thread-safe operation
   */
  size_t getConnectionCount() const;
};

#endif

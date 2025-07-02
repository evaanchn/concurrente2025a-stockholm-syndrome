// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "WorkerConnections.hpp"

// TODO(Andrey): make documentation
WorkerConnections::~WorkerConnections() {
  this->connections.clear();
}

Socket WorkerConnections::getRandomWorkerConnection() {
  std::lock_guard<std::mutex> lock(this->canAccessWorkerConnections);
  Socket selectedSocket;
  // If no connections are left, throw error
  if (this->connections.empty()) {
    throw std::runtime_error("no worker connections available");
  }
  // Generate random index
  unsigned seed = time(NULL) + clock() + pthread_self();
  size_t index = rand_r(&seed) % this->connections.size();
  selectedSocket = this->connections[index];  // Obtain socket
  return selectedSocket;
}

void WorkerConnections::removeSocket(Socket socket) {
  std::lock_guard<std::mutex> lock(this->canAccessWorkerConnections);
  // Find the index of the socket to remove
  size_t index = 0;
  for (; index < this->connections.size(); ++index) {
    if (this->connections[index] == socket) {
      break;  // Found the socket
    }
  }
  this->connections.erase(this->connections.begin() + index);
}

void WorkerConnections::addConnection(Socket& socket) {
  std::lock_guard<std::mutex> lock(this->canAccessWorkerConnections);
  this->connections.push_back(socket);
}

void WorkerConnections::stopWorkers() {
  std::lock_guard<std::mutex> lock(this->canAccessWorkerConnections);
  for (Socket& socket : this->connections) {
    // Skip if socket is not connected
    // if (!socket) {
    //   continue;
    // }
    // Send stop condition to worker
    socket << "-1\n";  // -1 is a stop condition
    socket.send();  // Send stop condition
    // Optionally, close the socket if needed
    // socket.close();
  }
}

bool WorkerConnections::hasConnections() const {
  return !this->connections.empty();
}

size_t WorkerConnections::getConnectionCount() const {
  return this->connections.size();
}

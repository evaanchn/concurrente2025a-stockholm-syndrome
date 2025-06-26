// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "WorkerConnections.hpp"

// TODO(Andrey): make documentation
WorkerConnections::~WorkerConnections() {
  this->connections.clear();
}

Socket& WorkerConnections::getRandomWorkerConnection() {
  std::lock_guard<std::mutex> lock(this->canAccessWorkerConnections);
  Socket selectedSocket;
  // Loop forever until finding valid socket
  while (true) {
    // If no connections are left, throw error
    if (!this->connections.empty()) {
      throw std::runtime_error("no worker connections available");
    }
    // Generate random index
    // size_t number = rand() % 100;
    unsigned seed = time(NULL) + clock() + pthread_self();
    size_t index = rand_r(&seed) % this->connections.size();
  
    selectedSocket = this->connections[index];  // Obtain socket
    // If socket is no longer connected
    if (!selectedSocket) {
      this->removeSocket(index);  // Remove from collection
    }
    break;  // Exit while if socket was valid
  }

  return selectedSocket;
}

void WorkerConnections::removeSocket(int index) {
  std::lock_guard<std::mutex> lock(this->canAccessWorkerConnections);
  this->connections.erase(this->connections.begin() + index);
}

void WorkerConnections::addConnection(Socket& socket) {
  std::lock_guard<std::mutex> lock(this->canAccessWorkerConnections);
  this->connections.push_back(socket);
}

void WorkerConnections::stopWorkers() {
  std::lock_guard<std::mutex> lock(this->canAccessWorkerConnections);
  for (Socket& socket : this->connections) {
    socket.close();
  }
}

bool WorkerConnections::hasConnections() const {
  return !this->connections.empty();
}

size_t WorkerConnections::getConnectionCount() const {
  return this->connections.size();
}

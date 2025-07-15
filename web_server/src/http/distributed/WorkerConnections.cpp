// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "WorkerConnections.hpp"

WorkerConnections::~WorkerConnections() {
  this->connections.clear();
}

Socket WorkerConnections::getRandomWorkerConnection() {
  this->canAccessWorkerConnections.lock();
  // If no connections are left, throw error
  if (this->connections.empty()) {
    this->canAccessWorkerConnections.unlock();
    throw std::runtime_error("no worker connections available");
  }
  // Generate random index
  const size_t index = Util::random(
      0, static_cast<int>(this->connections.size()));
  Socket selectedSocket = this->connections[index];  // Obtain socket
  this->canAccessWorkerConnections.unlock();
  return selectedSocket;
}

void WorkerConnections::removeSocket(Socket& socket) {
  this->canAccessWorkerConnections.lock();
  // Find the index of the socket to remove
  size_t index = 0;
  for (; index < this->connections.size(); ++index) {
    if (this->connections[index] == socket) {
      break;  // Found the socket
    }
  }
  this->connections.erase(this->connections.begin() + index);
  this->canAccessWorkerConnections.unlock();
}

void WorkerConnections::addConnection(Socket& socket) {
  this->canAccessWorkerConnections.lock();
  this->connections.push_back(socket);
  this->canAccessWorkerConnections.unlock();
}

void WorkerConnections::stopWorkers() {
  this->canAccessWorkerConnections.lock();
  for (Socket& socket : this->connections) {
    // Send stop condition to worker
    socket << "-1\n";  // -1 is a stop condition
    socket.send();  // Send stop condition
    // Optionally, close the socket if needed
    // socket.close();
  }
  this->canAccessWorkerConnections.unlock();
}

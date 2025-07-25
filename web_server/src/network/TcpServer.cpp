// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>
#include <utility>

#include "TcpServer.hpp"
#include "NetworkAddress.hpp"
#include "Socket.hpp"

TcpServer::TcpServer(const int connectionQueueCapacity)
  : connectionQueueCapacity(connectionQueueCapacity) {
  // Set initial values for searching for a suitable network address
  ::memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_PASSIVE;  // Fill with my local IP
  hints.ai_family = AF_UNSPEC;  // Use IPv4 or IPv6, whichever
  hints.ai_socktype = SOCK_STREAM;  // Use TCP
}

TcpServer::~TcpServer() {
  this->stopListening();
  this->freeNetworkAddress();
}

void TcpServer::listenForever(const char* port) {
  this->listenForConnections(port);
  this->acceptAllConnections();
}


void TcpServer::stopListening() {
  // An ugly way to stop incoming connections is closing the socket
  if (this->connectionRequestSocket >= 0) {
    // Disable reading from the socket, so no more conn requests are allowed
    ::shutdown(this->connectionRequestSocket, SHUT_RD);
    // Close and destroy the socket file descriptor, releasing its resources
    // this->closeSocket();
  }
}

void TcpServer::listenForConnections(const char* port) {
  // Fetch all available network addresses where we can listen with this port
  this->fetchAvailableAddresses(port);
  // Get a socket for the first available address that works for the given port
  int error = this->openConnectionRequestSocket(port);
  if (error == 0) {  // Success
    // Ask the operating system to create a queue and enqueue all connection
    // requests on the socket bounded to the listening port. The connection
    // requests within the queue will be waiting to be accepted or rejected
    error = ::listen(this->connectionRequestSocket
        , this->connectionQueueCapacity);
  }
  // If OS was unable to start listening for connections, raise an exception
  if (error) {
    this->closeSocket();
    this->freeNetworkAddress();
    throw std::runtime_error(std::string("could not listen port ") + port);
  }
}

void TcpServer::fetchAvailableAddresses(const char* port) {
  // This function is designed to be called once
  assert(this->availableAddresses == nullptr);
  // Fetch all available addresses and store results in object attributes
  const int error = ::getaddrinfo(nullptr, port, &this->hints
    , &this->availableAddresses);
  // If getaddrinfo failed, raise an exception
  if (error) {
    this->freeNetworkAddress();
    throw std::runtime_error(std::string("getaddrinfo: ")
       + ::gai_strerror(error));
  }
}

int TcpServer::openConnectionRequestSocket(const char* port) {
  assert(this->connectionRequestSocket == -1);
  // Traverse the available addresses and select the first one that works
  for (const struct addrinfo* address = this->availableAddresses; address;
      address = address->ai_next) {
    // Try to open a socket using this address result
    this->connectionRequestSocket = ::socket(address->ai_family
      , address->ai_socktype, address->ai_protocol);
    // If we could open the socket
    if (this->connectionRequestSocket >= 0) {
      // Allow the socket to reuse the local IP for other connections
      int yes = 1;
      int error = ::setsockopt(this->connectionRequestSocket, SOL_SOCKET
        , SO_REUSEADDR, &yes, sizeof yes);
      if (error == 0) {  // Success
        // Bind the socket to the port we passed in to getaddrinfo()
        // A socket must be bounded to a network port in order to listen from it
        error = ::bind(this->connectionRequestSocket, address->ai_addr
          , address->ai_addrlen);
        if (error == 0) {  // Success
          // This address is OK, stop searching and use its socket
          this->selectedAddress = address;
          return error;
        }
      } else {
        throw std::runtime_error("could not set reusing socket");
      }
      // We could not bind the port for this address, continue searching
      this->closeSocket();
    } else {
      // We could not open a socket for this address, continue searching
    }
    // Port is not available or cannot be opened
    return -1;
  }

  // Failed to get a socket for listening on localhost at this port
  this->connectionRequestSocket = -1;
  this->freeNetworkAddress();
  throw std::runtime_error(std::string("no available addresses for port ")
      + port);
}

void TcpServer::acceptAllConnections() {
  while (this->connectionRequestSocket >= 0) {
    this->acceptConnectionRequest();
  }
}

Socket TcpServer::acceptConnectionRequest() {
  assert(this->connectionRequestSocket >= 0);
  // This object represents the communication with the peer
  Socket client;
  // Wait for and accept only one connection request, the next available at the
  // head of the queue. accept() return the file descriptor for a new socket.
  // Therefore, after calling accept() there will be two sockets: the original
  // one for listening for new connection requests, and a new socket to send to
  // and receive messages from the accepted client process.
  // TODO(jhc): stop accept() and clean exit when signals (e.g. Ctrl+C) are sent
  // e.g: https://stackoverflow.com/a/35755340
  socklen_t clientAddressSize = sizeof(struct sockaddr_storage);
  const int file = ::accept(this->connectionRequestSocket
    , client.getSockAddr(), &clientAddressSize);
  // If connection failed, raise an exception
  if (file == -1) {
    throw std::runtime_error("could not accept client connection");
  }
  // The connection was accepted, fill the Socket object
  client.setSocketFileDescriptor(file);
  // Allow derivate classes to manage the connection
  this->handleClientConnection(client);
  // Return a copy of the socket if there are no derived classes
  return client;
}

NetworkAddress TcpServer::getNetworkAddress() const {
  assert(this->selectedAddress);
  return NetworkAddress(this->selectedAddress->ai_addr);
}

void TcpServer::freeNetworkAddress() {
  ::freeaddrinfo(this->availableAddresses);
  this->availableAddresses = nullptr;
}

void TcpServer::closeSocket() {
  if (this->connectionRequestSocket != -1) {
    ::close(this->connectionRequestSocket);
    this->connectionRequestSocket = -1;
  }
}

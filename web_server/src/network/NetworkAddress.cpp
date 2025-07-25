// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <stdexcept>
#include <string>

#include "NetworkAddress.hpp"

// Initialize with existing sockaddr structure (must be valid)
NetworkAddress::NetworkAddress(const struct sockaddr* address)
  : address{address} {
  assert(this->address);
}

// Converts binary IP address to human-readable string format
std::string NetworkAddress::getIP() const {
  assert(this->address);

  // Handle IPv4 addresses
  if (this->address->sa_family == AF_INET) {  // IPv4
    const struct sockaddr_in* address4 =
      reinterpret_cast<const struct sockaddr_in*>(this->address);
    char ip[INET_ADDRSTRLEN];
    ::inet_ntop(address4->sin_family, &address4->sin_addr, ip, INET_ADDRSTRLEN);
    return ip;
    // Handle IPv6 addresses
  } else if (address->sa_family == AF_INET6) {  // IPv6
    const struct sockaddr_in6* address6 =
      reinterpret_cast<const struct sockaddr_in6*>(this->address);
    char ip[INET6_ADDRSTRLEN];
    ::inet_ntop(address6->sin6_family, &address6->sin6_addr, ip
      , INET6_ADDRSTRLEN);
    return ip;
    // Invalid address family (should never happen due to assert)
  } else {
    assert(false);
    return "";
  }
}

// Extracts port number from network address (converts to host byte order)
port_t NetworkAddress::getPort() const {
  assert(this->address);

  // Handle IPv4 port extraction
  if (this->address->sa_family == AF_INET) {  // IPv4
    const struct sockaddr_in* address4 =
      reinterpret_cast<const struct sockaddr_in*>(this->address);
    return ntohs(address4->sin_port);
    // Handle IPv6 port extraction
  } else if (address->sa_family == AF_INET6) {  // IPv6
    const struct sockaddr_in6* address6 =
      reinterpret_cast<const struct sockaddr_in6*>(this->address);
    return ntohs(address6->sin6_port);
    // Invalid address family (should never happen due to assert)
  } else {
    assert(false);
    return 0;
  }
}

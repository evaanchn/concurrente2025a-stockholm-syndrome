// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef REQUEST_SERVER_HPP
#define REQUEST_SERVER_HPP

#include <vector>

#include "Producer.hpp"
#include "Socket.hpp"

#define REQUEST_BUFFER_LINES_COUNT 3

// forward declaration
class ConcurrentData;
class HttpApp;
struct DataUnit;
struct WorkUnit;

/// @brief Decomposes a ConcurrentData into data units
class RequestServer : public Producer<DataUnit*> {
  DISABLE_COPY(RequestServer);

 private:
  /// Reference to app chain in server
  std::vector<HttpApp*>& applications;
  /// Copy to the master connection
  Socket masterConnection;
  /// Amount of stop conditions to produce
  size_t stopConditionsToSend = 0;

 public:
  ///@brief Constructor
  /// @param applications Reference to the vector of HttpApp objects
  /// @param stopConditionsToSend Amount of stop conditions next entity needs
  RequestServer(std::vector<HttpApp*>& applications, Socket& masterConnection,
    size_t stopConditionsToSend);

  /// @brief Main procedure RequestServer thread will run
  /// @return EXIT_SUCCESS on return
  int run() override;

 private:
  /// @brief Consuming procedure
  /// @param data shared 
  void handleMasterConnection();

  /// @brief Read the request from the masterConnection connection and produce
  // a DataUnit
  DataUnit* readRequestFromMaster();
};

#endif  // REQUEST_SERVER_HPP

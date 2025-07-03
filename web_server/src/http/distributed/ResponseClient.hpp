// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef RESPONSE_CLIENT
#define RESPONSE_CLIENT

#include <vector>

#include "Consumer.hpp"
#include "Socket.hpp"
#include "TcpClient.hpp"


// forward declaration
class HttpApp;
struct DataUnit;
struct WorkUnit;

/// @brief TcpClient that connects to the master server
/// It sends results back to the master server after receiving them from
/// calculators.
class ResponseClient : public TcpClient, public Consumer<DataUnit*> {
  DISABLE_COPY(ResponseClient);

 private:
  /// Amount of stop conditions that this client must consume
  size_t pendingStopConditions = 0;

 public:
  /// @brief Constructor of the class
  /// @param pendingStopCondition Amount of stop conditions to consume
  explicit ResponseClient(size_t pendingStopCondition);

  /// @brief Destructor of the class
  ~ResponseClient() = default;

  /// @brief Connects to the master server
  /// @param serverIP The server's network address
  /// @param port The port number where the server is listening for connections
  /// @return A reference to the Socket object that can be used to send and
  /// receive information with the server
  /// @throw std::runtime_error if could not connect to the server
  Socket& connect(const char* serverIP, const char* port);

  /// @brief starts to consume all DataUnits completed
  int run() override;

  /// @brief Consumes a DataUnit and sends obtained result back to the master
  /// then, it deletes the DataUnit object since it is no longer needed.
  /// @param data Pointer to the DataUnit to be serialized and sent
  void consume(DataUnit* data) override;
};

#endif  // RESPONSE_CLIENT

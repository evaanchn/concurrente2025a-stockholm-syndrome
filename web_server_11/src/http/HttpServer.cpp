// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include "HttpApp.hpp"
#include "HttpServer.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Log.hpp"
#include "NetworkAddress.hpp"
#include "Socket.hpp"

const char* const usage =
  "Usage: webserv [port] [max_connections] [queue_capacity]\n"
  "\n"
  "  port        Network port to listen incoming HTTP requests, default "
    DEFAULT_PORT "\n"
  "  max_connections  Max amount of connections the server"
  " can attend concurrently\n"
  "  conn_queue_capacity  Max amount of connections that"
  " can be stored in queue\n";

HttpServer::HttpServer() {
}

HttpServer::~HttpServer() {
  delete this->socketsQueue;
  delete this->requestUnitsQueue;
}

HttpServer& HttpServer::getInstance() {
  static HttpServer server;
  return server;
}

void HttpServer::handleSignal(int signalID) {
  if (signalID == SIGINT || signalID == SIGTERM) {
    std::cout << "Stop: " << ((signalID == SIGINT) ? "SIGINT" : "SIGTERM")
      << " thread: " << std::this_thread::get_id() << "\n";
    HttpServer::getInstance().stopListening();
  }
}

void HttpServer::chainWebApp(HttpApp* application) {
  assert(application);
  this->applications.push_back(application);
}

int HttpServer::run(int argc, char* argv[]) {
  bool stopApps = false;
  try {
    if (this->analyzeArguments(argc, argv)) {
      stopApps = this->startServer();
      this->createThreads();
      this->connectQueues();
      this->startThreads();
      // Accept all client connections. The main process will get blocked
      // running this method and will not return. When HttpServer::stopListening
      // is called from another execution thread, an exception will be launched
      // that stops the acceptAllConnections() invocation and enters in the
      // catch below. Then, the main thread can continue stopping apps,
      /// finishing the server and any further cleaning it requires.
      this->acceptAllConnections();
    }
  } catch (const std::runtime_error& error) {
    std::cerr << "error: " << error.what() << std::endl;
  }
  // Destroy objects created by this server
  this->stopServer(stopApps);
  return EXIT_SUCCESS;
}

void HttpServer::listenForever(const char* port) {
  return TcpServer::listenForever(port);
}

void HttpServer::stop() {
  // Stop listening for incoming client connection requests. When stopListing()
  // method is called -maybe by a secondary thread-, the web server -running
  // by the main thread- will stop executing the acceptAllConnections() method.
  this->stopListening();
}

bool HttpServer::analyzeArguments(int argc, char* argv[]) {
  // Traverse all arguments
  for (int index = 1; index < argc; ++index) {
    const std::string argument = argv[index];
    if (argument.find("help") != std::string::npos) {
      std::cout << usage;
      return false;
    }
  }

  if (argc >= 2) {
    this->port = argv[1];
  }
  if (argc >= 3) {
    try {
      this->maxConnections = std::stoul(argv[2]);
    } catch(const std::invalid_argument& error) {
      std::cerr << "Warning: " << error.what()
        << " default values ​​were assigned" << std::endl;
      this->maxConnections = std::thread::hardware_concurrency();
    }
  }
  if (argc >= 4) {
    try {
      this->capacity = std::stoull(argv[3]);
    } catch(const std::invalid_argument& error) {
      std::cerr << "warning: " << error.what()
        << " default values ​​were assigned" << std::endl;
      this->capacity = SEM_VALUE_MAX;
    }
  }
  return true;
}

bool HttpServer::startServer() {
  // Set signal handler method from HttpConnectionHandler
  // TODO(any): save previous signal if required
  signal(SIGINT, HttpServer::handleSignal);
  signal(SIGTERM, HttpServer::handleSignal);
  // Start the log service
  Log::getInstance().start();
  // Start all web applications
  this->startApps();
  // Start waiting for connections
  this->listenForConnections(this->port);
  const NetworkAddress& address = this->getNetworkAddress();
  Log::append(Log::INFO, "webserver", "Listening on " + address.getIP()
    + " port " + std::to_string(address.getPort()));
  return true;
}

void HttpServer::startApps() {
  for (size_t index = 0; index < this->applications.size(); ++index) {
    this->applications[index]->start();
  }
}

void HttpServer::stopApps() {
  // Stop web applications. Give them an opportunity to clean up
  for (size_t index = 0; index < this->applications.size(); ++index) {
    this->applications[index]->stop();
  }
}

void HttpServer::stopServer(const bool stopApps) {
  // Join threads
  this->joinThreads();

  // Delete handlers
  this->deleteThreads();

  // If applications were started
  if (stopApps) {
    this->stopApps();
  }
  // Stop the log service
  Log::getInstance().stop();
}

void HttpServer::handleClientConnection(Socket& client) {
  this->socketsQueue->enqueue(client);
}

void HttpServer::createThreads() {
  // Reserve space in the handler vector and create a handler for
  // each allowed connection. Each handler is associated with the shared
  // socket queue to consume incoming client connections.
  this->handlers.reserve(this->maxConnections);
  for (size_t index = 0; index < this->maxConnections; ++index) {
    HttpConnectionHandler* handler =
      new HttpConnectionHandler(this->applications);
    this->handlers.push_back(handler);
  }

  // Create decomposer, which requires all handlers to stop before halting
  // After stopping, it notifies each calculator it must stop through
  // enqueued stop conditions in their queue
  this->decomposer = new Decomposer(/*pendindStopConditions*/ maxConnections
      , /*stopConditionsToSend*/ calculatorsAmount);
  this->decomposer->createOwnQueue(SEM_VALUE_MAX);

  // Reserve enough space for calculators
  this->calculators.reserve(this->calculatorsAmount);
  // Create each calculator and add to calculators vector
  for (size_t index = 0; index < this->calculatorsAmount; ++index) {
    Calculator* calculator = new Calculator();
    this->calculators.push_back(calculator);
  }

  // Response assembler must wait for all calculators to finish
  this->responseAssembler = new ResponseAssembler(/*pendingStopConditions*/
      calculatorsAmount);
  this->responseAssembler->createOwnQueue(SEM_VALUE_MAX);

  this->clientResponder = new ClientResponder();
  this->clientResponder->createOwnQueue(SEM_VALUE_MAX);
}

void HttpServer::connectQueues() {
  // Create the objects required to respond to the client
  this->socketsQueue = new Queue<Socket>(this->capacity);

  // Connection handlers will consume from the server's sockets queue
  for (size_t index = 0; index < this->maxConnections; ++index) {
    handlers[index]->setConsumingQueue(this->socketsQueue);
    handlers[index]->setProducingQueue(this->decomposer->getConsumingQueue());
  }

  this->requestUnitsQueue = new Queue<RequestUnit>(SEM_VALUE_MAX);

  this->decomposer->setProducingQueue(this->requestUnitsQueue);

  for (size_t index = 0; index < this->calculatorsAmount; ++index) {
    this->calculators[index]->setConsumingQueue(this->requestUnitsQueue);
    this->calculators[index]->
        setProducingQueue(this->responseAssembler->getConsumingQueue());
  }

  this->responseAssembler->setProducingQueue(this->clientResponder->
      getConsumingQueue());
}

void HttpServer::startThreads() {
  // Starts the connection handler threads to accept connections
  for (size_t index = 0; index < this->maxConnections; ++index) {
    this->handlers[index]->startThread();
  }
  this->decomposer->startThread();
  for (size_t index = 0; index < this->calculatorsAmount; ++index) {
    this->calculators[index]->startThread();
  }
  this->responseAssembler->startThread();
  this->clientResponder->startThread();
}

void HttpServer::joinThreads() {
  // Send stop conditions to handlers
  for (size_t i = 0; i < this->maxConnections; ++i) {
    this->socketsQueue->enqueue(Socket());
  }

  // Wait for thread objects to finish and join
  for (size_t index = 0; index < this->handlers.size(); ++index) {
    this->handlers[index]->waitToFinish();
  }

  this->decomposer->waitToFinish();

  for (size_t index = 0; index < this->calculatorsAmount; ++index) {
    this->calculators[index]->waitToFinish();
  }

  this->responseAssembler->waitToFinish();
  this->clientResponder->waitToFinish();
}

void HttpServer::deleteThreads() {
  // Free memory allocated for handler thread objects
  for (HttpConnectionHandler* handler : this->handlers) {
    delete handler;
  }
  this->handlers.clear();  // Clear pointers stored

  delete this->decomposer;
  this->decomposer = nullptr;

  for (Calculator* calculator : this->calculators) {
    delete calculator;
  }
  this->calculators.clear();  // Clear pointers stored

  delete this->responseAssembler;
  this->responseAssembler = nullptr;

  delete this->clientResponder;
  this->clientResponder = nullptr;
}

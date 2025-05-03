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

// TODO(you): Implement connection handlers argument
const char* const usage =
  "Usage: webserv [port] [handlers]\n"
  "\n"
  "  port        Network port to listen incoming HTTP requests, default "
    DEFAULT_PORT "\n"
  "  handlers     Number of connection handler theads\n";

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

HttpServer::HttpServer() {
}

HttpServer::~HttpServer() {
  delete this->queue;
}

void HttpServer::listenForever(const char* port) {
  return TcpServer::listenForever(port);
}

void HttpServer::stop() {
  // Stop listening for incoming client connection requests. When stopListing()
  // method is called -maybe by a secondary thread-, the web server -running
  // by the main thread- will stop executing the acceptAllConnections() method.
  // TODO(you): stopListening must be called from a signal handler, not here
  // done?
  this->stopListening();
}

void HttpServer::chainWebApp(HttpApp* application) {
  assert(application);
  this->applications.push_back(application);
}

int HttpServer::run(int argc, char* argv[]) {
  bool stopApps = false;
  try {
    if (this->analyzeArguments(argc, argv)) {
      // Create the objects required to respond to the client
      this->queue = new Queue<Socket>(this->capacity);
      stopApps = this->startServer();
      // TODO(us): move to a createHandlers method
      this->createHandlers();
      this->startHandlers();
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
  // TODO(you): Optionally log the main thread id
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
  // Send stop condition
  for (size_t i = 0; i < this->maxConnections; ++i) {
    this->queue->enqueue(Socket());
  }

  // Join threads
  this->stopHandlers();

  // If applications were started
  if (stopApps) {
    this->stopApps();
  }
  // Stop the log service
  Log::getInstance().stop();
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
    }catch(const std::invalid_argument& error) {
      std::cerr << "Warning: " << error.what()
        << " default values ​​were assigned" << std::endl;
      this->maxConnections = std::thread::hardware_concurrency();
    }
  }
  if (argc >= 4) {
    try {
      this->capacity = std::stoull(argv[3]);
    }catch(const std::invalid_argument& error) {
      std::cerr << "warning: " << error.what()
        << " default values ​​were assigned" << std::endl;
      this->capacity = SEM_VALUE_MAX;
    }
  }
  return true;
}

void HttpServer::handleClientConnection(Socket& client) {
  // TODO(you): Make this method concurrent. Store client connections (sockets)
  // into a collection (e.g thread-safe queue) and stop in web server

  this->queue->enqueue(client);
}

void HttpServer::createHandlers() {
  this->handlers.reserve(this->maxConnections);
  for (size_t index = 0; index < this->maxConnections; ++index) {
    HttpConnectionHandler* handler =
      new HttpConnectionHandler(this->applications);
    handler->setConsumingQueue(this->queue);
    this->handlers.push_back(handler);
  }
}

void HttpServer::startHandlers() {
  for (size_t index = 0; index < this->maxConnections; ++index) {
    this->handlers[index]->startThread();
  }
}

void HttpServer::stopHandlers() {
  for (size_t index = 0; index < this->maxConnections; ++index) {
    this->handlers[index]->waitToFinish();
    std::cout << "finished: " << index;
    delete this->handlers[index];
  }
}

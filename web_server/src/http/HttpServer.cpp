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

#include "Calculator.hpp"
#include "ClientResponder.hpp"
#include "Decomposer.hpp"
#include "Distributor.hpp"
#include "HttpConnectionHandler.hpp"
#include "ResponseAssembler.hpp"
#include "RequestClient.hpp"
#include "RequestServer.hpp"
#include "ResponseClient.hpp"
#include "WorkerConnectionHandler.hpp"
#include "WorkerConnections.hpp"
#include "MasterServer.hpp"

#define DISTRIBUTED true

const char* const usage =
  "Usage: webserver role max_worker_connections [port] [max_connections] "
  "[queue_capacity]\n\n"
  "  role        Role of the server, 'master' or 'worker''\n"
  "  max_worker_connections  Max amount of worker connections\n"
  "  port        Network port to listen incoming HTTP requests, default "
    DEFAULT_PORT "\n"
  "  max_connections  Max amount of connections the server"
  " can attend concurrently\n"
  "  conn_queue_capacity  Max amount of connections that"
  " can be stored in queue\n";

const char* const usage_worker =
  "Usage: webserver worker master_port master_ip\n\n"
  "  worker       Role of the server\n"
  "  master_ip    IP address of the master server\n"
  "  master_port  Port where the master server is listening for connections\n";

HttpServer::HttpServer() {
}

HttpServer::~HttpServer() {
  // Free memory, if needed
  if (this->socketsQueue) delete this->socketsQueue;
  if (this->dataUnitsQueue) delete this->dataUnitsQueue;
  if (this->workerConnectionsQueue) delete this->workerConnectionsQueue;
}

HttpServer& HttpServer::getInstance() {
  static HttpServer server;
  return server;
}

void HttpServer::handleSignal(int signalID) {
  if (signalID == SIGINT || signalID == SIGTERM) {
    std::cout << "Stop: " << ((signalID == SIGINT) ? "SIGINT" : "SIGTERM")
      << " thread: " << std::this_thread::get_id() << "\n";
    HttpServer::getInstance().stop();
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
      this->createQueues();
      this->connectQueues();
      this->startThreads();
      // Accept all client connections. The main process will get blocked
      // running this method and will not return. When HttpServer::stopListening
      // is called from another execution thread, an exception will be launched
      // that stops the acceptAllConnections() invocation and enters in the
      // catch below. Then, the main thread can continue stopping apps,
      /// finishing the server and any further cleaning it requires.
      this->acceptAllConnections();
    } else {
      // If analyzeArguments() returned false, run worker logic
      return this->runWorker(argc, argv);
    }
  } catch (const std::invalid_argument& error) {
    std::cerr << "error: " << error.what() << std::endl;
    std::cout << usage;
    return EXIT_FAILURE;
  } catch (const std::runtime_error& error) {
    std::cerr << "error: " << error.what() << std::endl;
  }
  // Destroy objects created by this server
  this->stopServer(stopApps);
  return EXIT_SUCCESS;
}

bool HttpServer::analyzeArguments(int argc, char* argv[]) {
  if (argc < 2) {
    throw std::invalid_argument("insufficient arguments");
  }

  // Traverse all arguments to check for 'help' specification
  for (int index = 1; index < argc; ++index) {
    const std::string argument = argv[index];
    if (argument.find("help") != std::string::npos) {
      throw std::invalid_argument("help requested");
    }
  }

  this->role = argv[1];  // Set role to analyze
  // If speified role is worker, discontinue analysis
  if (this->role == "worker") {
    return false;
  } else if (this->role != "master") {
    // If role is not master either, the argument is invalid
    throw std::invalid_argument("must specify the role of the server");
  }

  if (argc < 3) {
    throw std::invalid_argument("must specify max worker connections");
  }
  try {
    this->maxWorkerConnections = std::stoul(argv[2]);
  } catch(const std::invalid_argument& error) {
    throw std::invalid_argument("invalid worker connections");
  }
  if (argc >= 4) {
    this->port = argv[3];
  }
  if (argc >= 5) {
    try {
      this->maxConnections = std::stoull(argv[4]);
    } catch(const std::invalid_argument& error) {
      std::cerr << "warning: " << error.what()
        << " default values ​​were assigned" << std::endl;
      return false;
    }
  }
  if (argc >= 6) {
    try {
      this->queuesCapacity = std::stoull(argv[4]);
    } catch(const std::invalid_argument& error) {
      std::cerr << "warning: " << error.what()
        << " default values ​​were assigned" << std::endl;
      this->queuesCapacity = SEM_VALUE_MAX;
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

void HttpServer::listenForever(const char* port) {
  return TcpServer::listenForever(port);
}


void HttpServer::createThreads() {
  // Ensure that it's only called once, when there are no handlers
  assert(this->handlers.size() == 0);
  // Reserve space in the handler vector and create a handler for
  // each allowed connection. Each handler is associated with the shared
  // socket queue to consume incoming client connections.
  this->handlers.reserve(this->maxConnections);
  for (size_t index = 0; index < this->maxConnections; ++index) {
    HttpConnectionHandler* handler =
      new HttpConnectionHandler(this->applications);  // Pass reference to apps
    this->handlers.push_back(handler);
  }

  // Create decomposer, which requires all handlers to stop before halting
  // After stopping, it notifies each calculator it must stop through
  // enqueued stop conditions in their queue
  this->decomposer = new Decomposer(
      /*pendindStopConditions*/ this->maxConnections);

  this->distributor = new Distributor(this->workerConnections,
      /*stopConditionsToSend*/ this->calculatorsAmount);

  this->requestClient = new RequestClient(this->workerConnections,
      this->applications);

  // Reserve enough space for calculators
  this->calculators.reserve(this->calculatorsAmount);
  // Create each calculator and add to calculators vector
  for (size_t index = 0; index < this->calculatorsAmount; ++index) {
    Calculator* calculator = new Calculator();
    this->calculators.push_back(calculator);
  }

  // Create server in charge of listening for worker conenctions
  this->masterServer = new MasterServer(this->workerConnections,
      /*stopConditionsToSend*/ this->maxWorkerConnections);

  // Reserve space for worker connection handlers
  this->workerConnectionHandlers.reserve(this->maxWorkerConnections);
  // Create each worker connection handler and add to vector
  for (size_t index = 0; index < this->maxWorkerConnections; ++index) {
    WorkerConnectionHandler* workerHandler = new WorkerConnectionHandler(
      this->applications, this->workerConnections);
    this->workerConnectionHandlers.push_back(workerHandler);
  }

  // Response assembler must wait for all calculators and worker connection
  // handlers to finish
  this->responseAssembler = new ResponseAssembler(/*pendingStopConditions*/
    this->calculatorsAmount + this->maxWorkerConnections);
  // Create client responder with its own queue
  this->clientResponder = new ClientResponder();
}

void HttpServer::createQueues() {
  // Create the queue for handlers to get connections to work with
  this->socketsQueue = new Queue<Socket>(this->queuesCapacity);

  // Decomposer consumes from its own queue
  this->decomposer->createOwnQueue(SEM_VALUE_MAX);

  // Distributor has its own queue to consume from
  this->distributor->createOwnQueue(SEM_VALUE_MAX);

  // RequestClient has its own queue to consume from
  this->requestClient->createOwnQueue(SEM_VALUE_MAX);

  // Create queue between decomposers and data units handlers
  this->dataUnitsQueue
      = new Queue<DataUnit*>(this->queuesCapacity);

  // Create queue for worker connections
  this->workerConnectionsQueue
      = new Queue<Socket>(this->maxWorkerConnections);

  // Response assembler and client responder have their own queue
  this->responseAssembler->createOwnQueue(SEM_VALUE_MAX);
  this->clientResponder->createOwnQueue(SEM_VALUE_MAX);
}

void HttpServer::connectQueues() {
  // Connection handlers will consume from the server's sockets queue
  // and produce to the decomposer's queue
  for (size_t index = 0; index < this->maxConnections; ++index) {
    handlers[index]->setConsumingQueue(this->socketsQueue);
    handlers[index]->setProducingQueue(this->decomposer->getConsumingQueue());
  }

  // Decomposer produces to the unit's queue
  this->decomposer->setProducingQueue(this->distributor->getConsumingQueue());

  // Distributor produces for requestClient and into dataUnits queue
  this->distributor->registerRedirect(DISTRIBUTED,
    this->requestClient->getConsumingQueue());
  this->distributor->registerRedirect(!DISTRIBUTED, this->dataUnitsQueue);

  // RequestClient consumes from its queue and produces to distributor's queue
  this->requestClient->setProducingQueue(this->
    distributor->getConsumingQueue());

  // Each calculator consumes from the data units queue
  // And produces to the response assembler's queue
  for (size_t index = 0; index < this->calculatorsAmount; ++index) {
    this->calculators[index]->setConsumingQueue(this->dataUnitsQueue);
    this->calculators[index]->
        setProducingQueue(this->responseAssembler->getConsumingQueue());
  }

  // MasterServer
  this->masterServer->setProducingQueue(this->workerConnectionsQueue);

  // WorkerConnectionHandlers
  for (size_t index = 0; index < this->maxWorkerConnections; ++index) {
    this->workerConnectionHandlers[index]->
      setConsumingQueue(this->workerConnectionsQueue);
    this->workerConnectionHandlers[index]->
      setProducingQueue(this->responseAssembler->getConsumingQueue());
  }
  // Response assembler produces to client responder's queue
  this->responseAssembler->setProducingQueue(this->clientResponder->
    getConsumingQueue());
}

void HttpServer::startThreads() {
  // Starts the connection handler threads to accept connections
  for (size_t index = 0; index < this->maxConnections; ++index) {
    this->handlers[index]->startThread();
  }
  // Decomposer starts to wait for concurrent data to decompose
  this->decomposer->startThread();

  this->distributor->startThread();

  this->requestClient->startThread();
  // Calculators start to wait for concurrent units to process
  for (size_t index = 0; index < this->calculatorsAmount; ++index) {
    this->calculators[index]->startThread();
  }

  // Response assembler starts to wait for units done
  this->responseAssembler->startThread();

  // Start master server thread
  this->masterServer->startThread();

  for (size_t index = 0; index < this->maxWorkerConnections; ++index) {
    this->workerConnectionHandlers[index]->startThread();
  }

  // Client responder starts to wait for concurrent data to be done
  this->clientResponder->startThread();
}

void HttpServer::handleClientConnection(Socket& client) {
  this->socketsQueue->enqueue(client);
}

void HttpServer::stop() {
  // Stop listening for incoming client connection requests. When stopListing()
  // method is called -maybe by a secondary thread-, the web server -running
  // by the main thread- will stop executing the acceptAllConnections() method.
  this->stopListening();
  this->masterServer->stopListening();
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

void HttpServer::joinThreads() {
  // Send stop conditions to handlers
  for (size_t i = 0; i < this->maxConnections; ++i) {
    this->socketsQueue->enqueue(Socket());
  }
  // Wait for thread objects to finish and join
  for (size_t index = 0; index < this->handlers.size(); ++index) {
    this->handlers[index]->waitToFinish();
  }

  // Wait for decomposer, who sends a stop condition to distributor before halt
  this->decomposer->waitToFinish();
  // Wait after decomposer and request client sent their stop conditions
  this->distributor->waitToFinish();
  // Wait since it receives a stop condition from distributor first
  this->requestClient->waitToFinish();

  for (size_t index = 0; index < this->calculatorsAmount; ++index) {
    this->calculators[index]->waitToFinish();
  }

  // Stop the master server, which will stop accepting worker connections
  // this->masterServer->stop();
  this->masterServer->waitToFinish();

  for (size_t index = 0; index < this->maxWorkerConnections; ++index) {
    this->workerConnectionHandlers[index]->waitToFinish();
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

  delete this->distributor;
  this->distributor = nullptr;

  delete this->requestClient;
  this->requestClient = nullptr;

  for (Calculator* calculator : this->calculators) {
    delete calculator;
  }
  this->calculators.clear();  // Clear pointers stored

  delete this->masterServer;
  this->masterServer = nullptr;
  for (size_t index = 0; index < this->maxWorkerConnections; ++index) {
    delete this->workerConnectionHandlers[index];
  }
  this->workerConnectionHandlers.clear();  // Clear pointers stored
  delete this->responseAssembler;
  this->responseAssembler = nullptr;

  delete this->clientResponder;
  this->clientResponder = nullptr;
}

void HttpServer::stopApps() {
  // Stop web applications. Give them an opportunity to clean up
  for (size_t index = 0; index < this->applications.size(); ++index) {
    this->applications[index]->stop();
  }
}

// =============================== WORKER =====================================
int HttpServer::runWorker(int argc, char* argv[]) {
  bool successfulStart = false;
  int result = EXIT_SUCCESS;
  try {
    // Obtain necessary specifications from arguments
    if (this->analyzeWorkerArguments(argc, argv)) {
      // If unsuccessful because of throw, skip to catch
      successfulStart = this->startWorker();
      this->startWorkerThreads();
    }
  } catch (const std::invalid_argument& error) {
    std::cerr << "error: " << error.what() << std::endl;
    std::cout << usage_worker;
    return EXIT_FAILURE;
  } catch (const std::runtime_error& error) {
    std::cerr << "error: " << error.what() << std::endl;
    result = EXIT_FAILURE;
  }

  // Destroy objects created by this server
  this->stopWorker(successfulStart);
  return result;
}


bool HttpServer::analyzeWorkerArguments(int argc, char* argv[]) {
  if (argc < 4) {
    throw std::invalid_argument("not enough arguments");
  }
  this->masterIP = argv[2];  // Set master IP for worker
  this->masterPort = argv[3];  // Set master port for worker
  return true;
}

bool HttpServer::startWorker() {
  // Start the log service
  Log::getInstance().start();

  // Start all web applications
  this->startApps();

  // Create worker's thread objects and conenct their queues
  this->createWorkerThreads();
  this->createWorkerQueues();
  this->connectWorkerQueues();

  this->responseClient->connect(this->masterIP, this->masterPort);

  return true;
}


void HttpServer::createWorkerThreads() {
  // Create response client to send responses to master server
  this->responseClient = new ResponseClient(this->calculatorsAmount);

  // Reserve enough space for calculators
  this->calculators.reserve(this->calculatorsAmount);
  // Create each calculator and add to calculators vector
  for (size_t index = 0; index < this->calculatorsAmount; ++index) {
    Calculator* calculator = new Calculator();
    this->calculators.push_back(calculator);
  }

  // Create request server to listen for requests from master server
  this->requestServer = new RequestServer(this->applications,
    responseClient->getSocket(), this->calculatorsAmount);
}

void HttpServer::createWorkerQueues() {
  // Create the queue betwen request server and calculators
  this->dataUnitsQueue
      = new Queue<DataUnit*>(this->queuesCapacity);

  // Response client has its own queue to consume
  this->responseClient->createOwnQueue(this->queuesCapacity);
}

void HttpServer::connectWorkerQueues() {
  // RequestServer produces to calculators
  this->requestServer->setProducingQueue(this->dataUnitsQueue);

  // Each calculator consumes from the data units queue
  // And produces to the response client
  for (size_t index = 0; index < this->calculatorsAmount; ++index) {
    this->calculators[index]->setConsumingQueue(this->dataUnitsQueue);
    this->calculators[index]->
        setProducingQueue(this->responseClient->getConsumingQueue());
  }
}

void HttpServer::startWorkerThreads() {
  // Request server starts to wait for requests from master server
  this->requestServer->startThread();

  // Calculators start to wait for data units to process
  for (size_t index = 0; index < this->calculatorsAmount; ++index) {
    this->calculators[index]->startThread();
  }

  // Response client starts to wait for responses to send to master server
  this->responseClient->startThread();
}

void HttpServer::stopWorker(const bool normalStop) {
  // Join threads, if they were started correctly
  if (normalStop) {
    this->joinWorkerThreads();
  }

  // Delete handlers
  this->deleteWorkerThreads();

  // If applications were started, stop
  if (normalStop) {
    this->stopApps();
  }
  // Stop the log service
  Log::getInstance().stop();
}

void HttpServer::joinWorkerThreads() {
  // Wait for thread objects to finish and join
  this->requestServer->waitToFinish();

  for (size_t index = 0; index < this->calculatorsAmount; ++index) {
    this->calculators[index]->waitToFinish();
  }

  this->responseClient->waitToFinish();
}

void HttpServer::deleteWorkerThreads() {
  // Free memory allocated for handler thread objects

  delete this->requestServer;
  this->requestServer = nullptr;

  for (Calculator* calculator : this->calculators) {
    delete calculator;
  }
  this->calculators.clear();  // Clear pointers stored

  delete this->responseClient;
  this->responseClient = nullptr;
}

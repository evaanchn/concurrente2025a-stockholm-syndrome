// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <climits>
#include <csignal>
#include <mutex>
#include <vector>

#include "Queue.hpp"
#include "TcpServer.hpp"
#include "Thread.hpp"
#include "HttpConnectionHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

#define DEFAULT_PORT "8080"

class HttpApp;

/**
@brief Implements a minimalist web server.

A web server is a software that listens for client connections,
accept them, waits for requests of resources, and answers the requests. All
communication with clients are following the rules of the HTTP (HyperText
Transfer Protocol).

A HttpServer is a stand-alone object, you do not require to inherit a class.
A web server should be a generic software that may be used for many
applications. A web application is a piece of software that deals with a
specific domain, e.g: a marble shop, a video game, a newspaper, and so on.
Your HttpServer can be connected with an arbitrary number of web
applications. The following code shows how the server can answer requests
of two different applications, one for lottery results and other for pets.

```cpp
int main(int argc, char* argv[]) {
  // Create the web server
  HttpServer httpServer;
  // Create some web applications
  LotteryWebApp lotteryWebApp;
  PetsWebApp petsWebApp;
  // Register the web application(s) with the web server
  httpServer.chainWebApp(&lotteryWebApp);
  httpServer.chainWebApp(&petsWebApp);
  // Start the web server
  return httpServer.run(argc, argv);
}
```

In the previous code, when a request arrives, the server asks the lottery
application to examine the request. The lottery application will study the
HttpRequest fields (e.g: URI) to determine if the request is about lottery.
In such case, the web application returns true to the server indicating it
managed the request. If the application returns false, indicates the server
that the request was not intended for the lottery application, and the server
repeats the process with the following application in the chain: the pets
application. If no application manages the request, a 404 Not-found response
is sent to the client.
*/
class HttpServer : public TcpServer{
  DISABLE_COPY(HttpServer);

 protected:
  /// TCP port where this web server will listen for connections
  const char* port = DEFAULT_PORT;
  /// Chain of registered web applications. Each time an incoming HTTP request
  /// is received, the request is provided to each application of this chain.
  /// If an application detects the request is for it, the application will
  /// call the httpResponse.send() and the chain stops. If no web app serves
  /// the request, the not found page will be served.
  std::vector<HttpApp*> applications;

 private:
  /// Number of threads to handle connections
  unsigned int maxConnections = std::thread::hardware_concurrency();
  /// queue default capacity
  uint64_t capacity = SEM_VALUE_MAX;
  /// socket producing queue
  Queue<Socket>* queue = nullptr;
  /// socket consumers
  std::vector<HttpConnectionHandler*> handlers;

 public:
  // Unique (singleton pattern) instance of the server
  static HttpServer& getInstance();


  /// @brief Handdles signals assigned by main thread
  /// @details gets Server instance and call the stop method to close it
  /// @param signalID macro from csignal library
  /// SIGINT: ctrl+c executed by the running enviroment
  /// SIGTERM: kill [PID] executed by a program specifiyng current proccess id
  static void handleSignal(int signalID);

  void createHandlers();
  void startHandlers();
  void stopHandlers();

  /// Destructor
  ~HttpServer();
  /// Registers a web application to the chain
  void chainWebApp(HttpApp* application);
  /// Start the web server for listening client connections and HTTP requests
  int run(int argc, char* argv[]);
  /// Indefinitely listen for client connection requests and accept all of them.
  /// For each accepted connection request, the virtual onConnectionAccepted()
  /// will be called. Inherited classes must override that method
  void listenForever(const char* port);
  /// @brief To be called by a HttpConnectionHandler that got a signal to
  /// close the server
  /// @details thread safe (mutex)
  void stop();

 protected:
  /// Constructor (not public for singleton pattern)
  HttpServer();
  /// Analyze the command line arguments
  /// @return true if program can continue execution, false otherwise
  bool analyzeArguments(int argc, char* argv[]);
  /// Start the web server. Create other objects required to respond to clients.
  /// @return true if apps were started
  bool startServer();
  /// Stop the web server. The server may stop not immediately. It will stop
  /// for listening further connection requests at once, but pending HTTP
  /// requests that are enqueued will be allowed to finish
  void stopServer(const bool stopApps);
  /// Start all registered applications, given them a chance to build their
  /// data structures just before starting to run
  void startApps();
  /// Stop all running applications, given them a chance to clean their data
  /// structures
  void stopApps();
  /// This method is called each time a client connection request is accepted.
  void handleClientConnection(Socket& client) override;
  /// Called each time an HTTP request is received. Web server should analyze
  /// the request object and assemble a response with the response object.
  /// Finally send the response calling the httpResponse.send() method.
  /// @return true on success and the server will continue handling further
  /// HTTP requests, or false if server should stop accepting requests from
  /// this client (e.g: HTTP/1.0)
  /// Sends a page for a non found resource in this server. This method is
  /// called if none of the registered web applications handled the request.
  /// If you want to override this method, create a web app, e.g NotFoundWebApp
  /// that reacts to all URIs, and chain it as the last web app
};

#endif  // HTTPSERVER_H

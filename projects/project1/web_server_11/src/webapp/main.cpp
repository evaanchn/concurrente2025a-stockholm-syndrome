// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0
// Serial web server's initial code for parallelization

#ifdef WEBSERVER

#include "HttpServer.hpp"
#include "HomeWebApp.hpp"
#include "FactWebApp.hpp"
#include "GoldbachWebApp.hpp"
#include "NotFoundWebApp.hpp"


// TODO(you): Register a signal handler for Ctrl+C and kill, and stop the server
// TODO(you): Make your signal handler to print the thread id running it

/// Start the web server
int main(int argc, char* argv[]) {
  // Create web applications
  HomeWebApp homeWebApp;
  FactWebApp factWebApp;
  GoldbachWebApp goldbachWebApp;
  NotFoundWebApp notFoundWebApp;
  // Register the web application(s) with the web server
  HttpServer::getInstance().chainWebApp(&homeWebApp);
  HttpServer::getInstance().chainWebApp(&factWebApp);
  HttpServer::getInstance().chainWebApp(&goldbachWebApp);
  HttpServer::getInstance().chainWebApp(&notFoundWebApp);
  // Run the web server
  return HttpServer::getInstance().run(argc, argv);
}

#endif  // WEBSERVER

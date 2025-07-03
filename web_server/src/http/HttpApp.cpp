// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#include <string>

#include "HttpApp.hpp"

void HttpApp::start() {
  // Default base class implementation does nothing
}

void HttpApp::stop() {
  // Default base class implementation does nothing
}

ConcurrentData* HttpApp::createConcurrentData(HttpRequest&, HttpResponse&,
    const size_t) {
  return nullptr;  // Returns nullptr by default, unless is concurrent app
}

std::string HttpApp::serializeRequest(DataUnit*) {
  return "";   // Returns empty by default, unless is concurrent app
}

DataUnit* HttpApp::deserializeResponse(std::string) {
  return nullptr;  // Returns nullptr by default, unless is concurrent app
}

DataUnit* HttpApp::deserializeRequest(std::string) {
  return nullptr;  // Returns nullptr by default, unless is concurrent app
}

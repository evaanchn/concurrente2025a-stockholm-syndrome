// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#include "HttpApp.hpp"

void HttpApp::start() {
  // Default base class implementation does nothing
}

void HttpApp::stop() {
  // Default base class implementation does nothing
}

RequestData* HttpApp::createRequestData(HttpRequest&, HttpResponse&) {
  return nullptr;  // Returns nullptr by default, unless is concurrent app
}

// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "GoldbachWebApp.hpp"
#include "GoldbachData.hpp"
#include "HomeWebApp.hpp"

GoldbachWebApp::GoldbachWebApp()
: CalcWebApp("/goldbach", "/goldbach?number=", "Goldbach sums") {
}

ConcurrentData* GoldbachWebApp::createConcurrentData(HttpRequest& httpRequest
    , HttpResponse& httpResponse, const size_t appIndex) {
  // Serve header of the html
  HomeWebApp::serveHeader(httpResponse, this->title);
  // Create a GoldbachData object to store concurrent data
  GoldbachData* data = new GoldbachData(httpRequest, httpResponse, appIndex);
  if (data) {
    // Parse the request to extract queries
    this->parseRequest(httpRequest, data->getQueries());
    // If no queries were parsed, send response and delete data
    if (data->getQueries().empty()) {
      data->respond();
      // deallocate data
      delete data;
      return nullptr;
    }
    data->updatePending();
  }
  return data;
}

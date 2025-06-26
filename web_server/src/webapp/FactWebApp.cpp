// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "FactWebApp.hpp"
#include "PrimeFactData.hpp"
#include "HomeWebApp.hpp"

FactWebApp::FactWebApp()
: CalcWebApp("/fact", "/fact?number=", "Prime Factorization") {
}

ConcurrentData* FactWebApp::createConcurrentData(HttpRequest& httpRequest
    , HttpResponse& httpResponse, const size_t appIndex) {
  // Serve header of the html
  HomeWebApp::serveHeader(httpResponse, this->title);
  // Create a PrimeFactData object to store data
  PrimeFactData* data = new PrimeFactData(httpRequest, httpResponse, appIndex);
  if (data) {
    // Parse the request to extract queries into data
    this->parseRequest(httpRequest, data->getQueries());
    // If no queries were parsed, send response and delete data
    if (data->getQueries().empty()) {
      data->respond();
      // deallocate tData
      delete data;
      return nullptr;
    }
    data->updatePending();
  }
  return data;
}

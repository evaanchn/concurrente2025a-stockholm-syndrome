// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "FactWebApp.hpp"
#include "PrimeFactCalculator.hpp"

FactWebApp::FactWebApp()
: CalcWebApp("/fact", "/fact?number=", "Prime Factorization") {
}

RequestData* FactWebApp::createRequestData(HttpRequest& httpRequest
    , HttpResponse& httpResponse) {
  // Serve header of the html
  HomeWebApp::serveHeader(httpResponse, this->title);
  // Create a FactRequestData object to store the request data
  FactRequestData* requestData = new FactRequestData(httpRequest, httpResponse);
  if (requestData) {
    // Parse the request to extract queries
    this->parseRequest(httpRequest, requestData->getQueries());
    // If no queries were parsed, send response and delete requestData
    if (requestData->getQueries().empty()) {
      requestData->respond();
      // deallocate requestData
      delete requestData;
      return nullptr;
    }
    requestData->updatePending();
  }
  return requestData;
}

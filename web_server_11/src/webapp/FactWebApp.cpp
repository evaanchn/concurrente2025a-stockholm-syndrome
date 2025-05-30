// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "FactWebApp.hpp"
#include "PrimeFactCalculator.hpp"

FactWebApp::FactWebApp()
: CalcWebApp("/fact", "/fact?number=", "Prime Factorization") {
}

RequestData* FactWebApp::createRequestData(HttpRequest& httpRequest
    , HttpResponse& httpResponse) {
  FactRequestData* requestData = new FactRequestData(httpRequest, httpResponse);
  if (requestData) {
    // Parse the request to extract queries
    this->parseRequest(httpRequest, requestData->getHttpResponse(),
      requestData->getQueries());
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

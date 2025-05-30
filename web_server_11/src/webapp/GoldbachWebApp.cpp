// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "GoldbachWebApp.hpp"
#include "GoldbachRequestData.hpp"

GoldbachWebApp::GoldbachWebApp()
: CalcWebApp("/goldbach", "/goldbach?number=", "Goldbach sums") {
}

RequestData* GoldbachWebApp::createRequestData(HttpRequest& httpRequest) {
  GoldbachRequestData* requestData = new GoldbachRequestData(httpRequest);
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

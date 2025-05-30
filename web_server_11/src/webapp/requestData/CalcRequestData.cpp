// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include <iostream>
#include <vector>

#include "CalcRequestData.hpp"
#include "CalcWebApp.hpp"

CalcRequestData::CalcRequestData(HttpRequest& httpRequest
  , ConcurrentApp* concurrentApp) : RequestData(httpRequest, concurrentApp) {
}

std::vector<int64_t>& CalcRequestData::getQueries() {
  return this->queries;
}

void CalcRequestData::updatePending() {
  // Update pending queries count and allocate results vector
  this->pendingQueries = this->queries.size();
  this->results.resize(this->queries.size());
}

std::vector<RequestUnit> CalcRequestData::decompose() {
  std::vector<RequestUnit> requestUnits;
  size_t index = 0;
  // Fill request units vector
  for (size_t i = 0; i < this->pendingQueries; ++i) {
    requestUnits.push_back(RequestUnit(this, index));
    ++index;
  }
  return requestUnits;
}

void CalcRequestData::respond() {
  CalcWebApp* calcWebApp = dynamic_cast<CalcWebApp*>(this->concurrentApp);
  if (calcWebApp) {
    // format into httpResponse body
    calcWebApp->formatResponse(this->results, this->httpResponse);
  } else {
    std::cerr << "Error: concurrentAppp could not be casted to calcWebApp."
      << std::endl;
    throw std::runtime_error("Invalid concurrentApp type.");
  }
  // send to client
  this->httpResponse.send();
}

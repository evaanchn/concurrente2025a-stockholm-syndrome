// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "CalcRequestData.hpp"
#include "CalcWebApp.hpp"

CalcRequestData::CalcRequestData(HttpRequest& httpRequest
  , ConcurrentApp* concurrentApp) : RequestData(httpRequest, concurrentApp) 
  , appCalculator(nullptr) {
}

CalcRequestData::~CalcRequestData() {
  delete this->appCalculator;
  this->appCalculator = nullptr;
}

std::vector<int64_t>& CalcRequestData::getQueries() {
  return this->queries;
}

void CalcRequestData::updatePending() {
  this->pendingQueries = this->queries.size();
  this->results.resize(this->queries.size());
}

std::vector<RequestUnit> CalcRequestData::decompose() {
  std::vector<RequestUnit> requestUnits;
  size_t index = 0;
  for (const auto& queries : this->queries) {
    requestUnits.push_back(RequestUnit(this, index));
    ++index;
  }
  return requestUnits;
}

void CalcRequestData::processQuery(size_t index) {
  if (this->appCalculator) {
    // processNumber(query, resultsVector)
    this->appCalculator->processNumber(this->queries[index],
        this->results[index]);
  } else {
    throw std::runtime_error("AppCalculator is not set.");
  }
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

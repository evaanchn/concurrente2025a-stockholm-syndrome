// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include <iostream>
#include <vector>

#include "CalcRequestData.hpp"
#include "CalcWebApp.hpp"

CalcRequestData::CalcRequestData(HttpRequest& httpRequest
    , HttpResponse& httpResponse)
    : RequestData(httpRequest, httpResponse)  {
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

void CalcRequestData::formatResponse(
  std::vector<std::vector<int64_t>>& results, HttpResponse& httpResponse) {
  // For each result, build the response
  for (auto& result : results) {
    this->buildResult(result, httpResponse);
  }
  // End ordered list of results
  httpResponse.body()
    << "      </div>\n"
    << "      <a href='/' class='back-button'>Back</a>\n"
    << "    </div>\n"
    << "  </div>\n"
    << "</body>\n"
    << "</html>\n";
}

void CalcRequestData::respond() {
  // format into httpResponse body
  this->formatResponse(this->results, this->httpResponse);
  // send to client
  this->httpResponse.send();
}

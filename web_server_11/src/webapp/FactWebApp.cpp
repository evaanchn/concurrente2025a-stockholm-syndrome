// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "FactWebApp.hpp"
#include "PrimeFactCalculator.hpp"

FactWebApp::FactWebApp()
: CalcWebApp("/fact", "/fact?number=", "Prime Factorization") {
}

void FactWebApp::buildResult(std::vector<int64_t>& result
  , HttpResponse& httpResponse) {
  // result contains the value at the first position and is followed by its
  // factors, in "factor, amount" form
  int64_t primeFactorsCount = (result.size() - 1) / 2;
  int64_t lastFactor = result.size() - 2;
  httpResponse.body()
    << "        <div class='number-result'>\n"
    << "          <span class='number-value'>" << result.at(0) << "</span> = \n"
    << "          <span class='factorization'>";

  if (primeFactorsCount == 0) {
    httpResponse.body() << "<span class='err'>Invalid number</span>";
  } else {
    for (int64_t factIndex = 1; factIndex < primeFactorsCount * 2 + 1;
        factIndex += 2) {
      httpResponse.body()
        << "<span class='factor'>" << result.at(factIndex) << "</span>";
      if (result.at(factIndex + 1) != 1) {
        httpResponse.body()
          << "<sup class='exponent'>"
          << result.at(factIndex + 1)
          << "</sup>";
      }
      // Avoid printing a multiplication symbol after last prime factor
      if (factIndex < lastFactor) {
        httpResponse.body() << " × ";
      }
    }
  }

  httpResponse.body()
    << "          </span>\n"
    << "        </div>\n";
}

RequestData* FactWebApp::createRequestData(HttpRequest& httpRequest) {
  FactRequestData* requestData = new FactRequestData(httpRequest, this);
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

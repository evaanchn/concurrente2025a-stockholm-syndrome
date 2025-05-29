// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "GoldbachWebApp.hpp"
#include "GoldbachRequestData.hpp"

GoldbachWebApp::GoldbachWebApp()
: CalcWebApp("/goldbach", "/goldbach?number=", "Goldbach sums") {
}

void GoldbachWebApp::buildResult(std::vector<int64_t>& result
    , HttpResponse& httpResponse) {
  // checks wheter each sum() has two (even value) or three(odd) operands
  size_t sumOperands = (result.front() % 2 == 0) ? 2 : 3;
  // result contains the value at the first position and is followed by its
  // sums
  size_t sumsCount = (result.size() - 1) / sumOperands;
  try {
    httpResponse.body()
      << "    <div class='results-container'>\n"
      << "      <h2 class='result-title'>Goldbach Sums Results</h2>\n"
      << "      <div class='result-item'>\n"
      << "        <div class='number-result'>\n"
      << "          <span class='number-value'>" << result.front()
      << "</span>: \n"
      << "          <span class='sums-count'>" << sumsCount << " sums</span>\n"
      << "        </div>";
    if (result.front() < 0 && sumsCount > 0) {
      httpResponse.body()
        << "        <div class='sums-grid'>";
      for (size_t sumIndex = 1; sumIndex <= result.size() - sumOperands;
        sumIndex += sumOperands) {
        httpResponse.body()
          << "          <div class='sum-item'>";
        for (size_t operandIndex = sumIndex;
          operandIndex < sumOperands + sumIndex; ++operandIndex) {
          httpResponse.body() << result.at(operandIndex);
          if (operandIndex != sumOperands + sumIndex - 1) {
            httpResponse.body() << " + ";
          }
        }
        httpResponse.body() << "</div>";
      }
      httpResponse.body() << "        </div>";
    }
    httpResponse.body()
      << "      </div>\n"
      << "      <a href='/' class='back-button'>Back</a>\n"
      << "    </div>\n"
      << "  </div>\n"
      << "</body>\n"
      << "</html>\n";
  } catch (std::runtime_error& error) {
    httpResponse.body()
      << "    <li class=err>"
      <<        error.what()
      << "    </li>";
  }
}

void GoldbachWebApp::sumsCountResponse(int64_t sumsCount
  , HttpResponse& httpResponse) {
  httpResponse.body() << ": <span style='color: var(--accent);'>"
                     << sumsCount << "</span> sums";
}

void GoldbachWebApp::sumsResponse(std::vector<int64_t>& sums
  , HttpResponse& httpResponse) {
  size_t sumOperands = ((sums.front() % 2) == 0) ? 2 : 3;

  httpResponse.body() << "<div class='sums-list'>";
  for (size_t sumIndex = 1; sumIndex <= sums.size() - sumOperands
      ; sumIndex += sumOperands) {
    httpResponse.body() << "<div class='sum-item'>";
    for (size_t operandIndex = sumIndex; operandIndex < sumOperands + sumIndex
        ; ++operandIndex) {
      httpResponse.body() << sums.at(operandIndex);
      if (operandIndex != sumOperands + sumIndex - 1)
        httpResponse.body() << " + ";
    }
    httpResponse.body() << "</div>";
  }
  httpResponse.body() << "</div>";
}

RequestData* GoldbachWebApp::createRequestData(HttpRequest& httpRequest) {
  GoldbachRequestData* requestData = new GoldbachRequestData(httpRequest, this);
  if (requestData) {
    this->parseRequest(httpRequest, requestData->getHttpResponse(),
      requestData->getQueries());
  }
  requestData->updatePending();
  return requestData;
}
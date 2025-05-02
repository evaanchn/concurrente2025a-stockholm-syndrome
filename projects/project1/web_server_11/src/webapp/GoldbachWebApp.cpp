// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "GoldbachWebApp.hpp"

GoldbachWebApp::GoldbachWebApp()
: CalcWebApp("/goldbach", "/goldbach?number=", "Goldbach sums") {
}

void GoldbachWebApp::buildResponse(const int64_t value
  , HttpResponse& httpResponse) {
  std::vector<int64_t> sums;
  int64_t sumsCount = 0;
  GoldbachCalculator myGoldbach;
  sumsCount = myGoldbach.processNumber(std::abs(value), sums);
  httpResponse.body()
    << "    <div class='results-container'>\n"
    << "      <h2 class='result-title'>Goldbach Sums Results</h2>\n"
    << "      <div class='result-item'>\n"
    << "        <div class='number-result'>\n"
    << "          <span class='number-value'>" << value << "</span>: \n"
    << "          <span class='sums-count'>" << sumsCount << " sums</span>\n"
    << "        </div>";
  if (value < 0 && sumsCount > 0) {
    size_t sumOperands = (sums.front() % 2 == 0) ? 2 : 3;
    httpResponse.body()
      << "        <div class='sums-grid'>";
    for (size_t sumIndex = 1; sumIndex <= sums.size() - sumOperands;
      sumIndex += sumOperands) {
      httpResponse.body()
        << "          <div class='sum-item'>";
      for (size_t operandIndex = sumIndex;
        operandIndex < sumOperands + sumIndex; ++operandIndex) {
        httpResponse.body() << sums.at(operandIndex);
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

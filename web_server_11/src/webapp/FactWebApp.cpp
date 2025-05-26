// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "FactWebApp.hpp"

FactWebApp::FactWebApp()
: CalcWebApp("/fact", "/fact?number=", "Prime Factorization") {
}

void FactWebApp::buildResult(std::vector<int64_t>& result
  , HttpResponse& httpResponse) {
  // result contains the value at the first position and is followed by its
  // factors
  int64_t primeFactorsCount = result.size() - 1;

  httpResponse.body()
    << "    <div class='results-container'>\n"
    << "      <h2 class='result-title'>Prime Factorization Results</h2>\n"
    << "      <div class='result-item'>\n"
    << "        <div class='number-result'>\n"
    << "          <span class='number-value'>" << result.at(0) << "</span> = \n"
    << "          <span class='factorization'>";

  if (primeFactorsCount == 0) {
    httpResponse.body() << "<span class='err'>Invalid number</span>";
  } else {
    for (int64_t factIndex = 1; factIndex < primeFactorsCount * 2;
      factIndex += 2) {
      httpResponse.body()
        << "<span class='factor'>" << result.at(factIndex) << "</span>";
      if (result.at(factIndex + 1) != 1) {
        httpResponse.body()
          << "<sup class='exponent'>"
          << result.at(factIndex + 1)
          << "</sup>";
      }
      if (factIndex < (primeFactorsCount * 2) - 2) {
        httpResponse.body() << " × ";
      }
    }
  }

  httpResponse.body()
    << "          </span>\n"
    << "        </div>\n"
    << "      </div>\n"
    << "      <a href='/' class='back-button'>Back</a>\n"
    << "    </div>\n"
    << "  </div>\n"
    << "</body>\n"
    << "</html>\n";
}

void FactWebApp::factResponse(std::vector<int64_t>& primeFactors
  , const int64_t value, int64_t primeFactorsCount
  , HttpResponse& httpResponse) {
  httpResponse.body()
    << "        <li><strong>" << value << "</strong> = ";

  for (int64_t factIndex = 0; factIndex < primeFactorsCount * 2
    ; factIndex += 2) {
    httpResponse.body() << "<span style='color: var(--primary);'>"
                       << primeFactors.at(factIndex) << "</span>";

    if (primeFactors.at(factIndex + 1) != 1) {
      httpResponse.body()
        << "<sup style='color: var(--accent);'>"
        << primeFactors.at(factIndex + 1) << "</sup>";
    }

    if (factIndex < (primeFactorsCount * 2) - 2) {
      httpResponse.body() << " × ";
    }
  }
  httpResponse.body()
    << "</li>\n";
}

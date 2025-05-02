// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "FactWebApp.hpp"

FactWebApp::FactWebApp()
: CalcWebApp("/fact", "/fact?number=", "Prime Factorization") {
}

void FactWebApp::buildResponse(const int64_t value
  , HttpResponse& httpResponse) {
  std::vector<int64_t> primeFactors;
  int64_t primeFactorsCount = 0;
  PrimeFactCalculator myPrimeFact;
  primeFactorsCount = myPrimeFact.processNumber(std::abs(value), primeFactors);

  httpResponse.body()
    << "    <div class='results-container'>\n"
    << "      <h2 class='result-title'>Prime Factorization Results</h2>\n"
    << "      <div class='result-item'>\n"
    << "        <div class='number-result'>\n"
    << "          <span class='number-value'>" << value << "</span> = \n"
    << "          <span class='factorization'>";

  if (primeFactorsCount == 0) {
    httpResponse.body() << "<span class='err'>Invalid number</span>";
  } else {
    for (int64_t factIndex = 0; factIndex < primeFactorsCount * 2;
      factIndex += 2) {
      httpResponse.body()
        << "<span class='factor'>" << primeFactors.at(factIndex) << "</span>";
      if (primeFactors.at(factIndex + 1) != 1) {
        httpResponse.body()
          << "<sup class='exponent'>"
          << primeFactors.at(factIndex + 1)
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

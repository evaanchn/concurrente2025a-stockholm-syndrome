// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include <iostream>
#include <string>
#include <vector>

#include "CalcRequestData.hpp"
#include "RequestUnit.hpp"
#include "Util.hpp"

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

const std::vector<std::string> CalcRequestData::getURIValues() {
  // Replace %xx hexadecimal codes by their ASCII symbols
  const std::string& uri = Util::decodeURI(httpRequest.getURI());
  // Numbers were asked in the form "/appPrefix/123,45,-7899" or
  // "/[appPrefix]?number=13"
  // Determine the position where numbers start
  size_t numbersStart = uri.find('=');  // Start after '='
  if (numbersStart == std::string::npos) {
    // start after last '/'
    if ((numbersStart = uri.rfind('/')) == std::string::npos) {
      // If no '=' or '/' found, return empty vector
      return {};
    }
  }
  ++numbersStart;  // Move to the next character after '=' or '/'
  // split the numbers by commas
  const std::vector<std::string>& texts =
      Util::split(uri.substr(numbersStart), ",", true);
  return texts;
}

void CalcRequestData::formatResponse() {
  // Obtain the values from the URI
  std::vector<std::string> valueList = this->getURIValues();
  // start ordered list of results
  this->httpResponse.body()
    << "    <div class='results-container'>\n"
    // << "      <h2 class='result-title'>" << this->title << "</h2>\n"
    << "      <div class='result-item'>\n";
  // as many results as valid values where processed
  size_t resultsIndex = 0;
  // For each requested value, build the response
  for (size_t index = 0; index < valueList.size(); ++index) {
    try {
      // Convert the text to a number. Provide an error message if not
      size_t end = 0;
      const int64_t value = std::stoll(valueList[index], &end);
      if (end != valueList[index].length()) {
        throw std::runtime_error("invalid number " + valueList[index]);
      }
      // If the value is valid, build the result
      // there is as many results as valid values where processed
      this->buildResult(value, this->results[resultsIndex]);
      ++resultsIndex;
    } catch (const std::exception& exception) {
      // Text was not a valid number, report an error to user
      this->httpResponse.body() << "        <div class='number-result'>\n"
        << "          <span class='number-value'>" << valueList[index]
        << "</span> = \n"
        << "          <span class='factorization'>"
        << "<span class='err'>Invalid number</span>"
        << "          </span>\n"
        << "        </div>\n";
    }
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
  this->formatResponse();
  // send to client
  this->httpResponse.send();
}

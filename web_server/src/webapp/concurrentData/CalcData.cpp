// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "CalcData.hpp"
#include "DataUnit.hpp"
#include "Util.hpp"

CalcData::CalcData(HttpRequest& httpRequest,
    HttpResponse& httpResponse, const size_t appIndex) :
    ConcurrentData(httpRequest, httpResponse, appIndex) {
}

std::vector<int64_t>& CalcData::getQueries() {
  return this->queries;
}

void CalcData::updatePending() {
  // Update pending queries count and allocate results vector
  this->pendingQueries = this->queries.size();
  this->results.resize(this->queries.size());
}

std::string CalcData::serializeQuery(size_t queryIndex) const {
  assert(queryIndex < this->queries.size());
  // Serialize the query as a string
  return std::to_string(this->queries[queryIndex]);
}

std::string CalcData::serializeResult(size_t queryIndex) const {
  assert(queryIndex < this->results.size());
  std::string resultBuffer;
  // results prefix
  resultBuffer += this->resultsPrefix;
  for (const int64_t& value : this->results[queryIndex]) {
    resultBuffer += std::to_string(value) + ",";
  }
  // Remove the last space if there are results
  if (!resultBuffer.empty()) {
    resultBuffer.pop_back();
  }
  return resultBuffer;
}

void CalcData::deserializeResult(const size_t resultIndex
    , std::string& queryResult) {
  assert(resultIndex < this->results.size());
  assert(!queryResult.empty());

  size_t numbersStart = queryResult.find(this->resultsPrefix) +
      this->resultsPrefix.length();
  // Split the result string by spaces
  const std::vector<std::string>& values =
    Util::split(queryResult.substr(numbersStart), ",", true);

  // Convert each value to int64_t and store in results
  for (const std::string& value : values) {
    try {
      this->results[resultIndex].push_back(std::stoll(value));
    } catch (const std::exception& exception) {
      throw std::runtime_error("Invalid result value: " + value);
    }
  }
}

std::vector<DataUnit*> CalcData::decompose() {
  std::vector<DataUnit*> dataUnits;
  size_t index = 0;
  // Fill data units vector
  for (size_t i = 0; i < this->pendingQueries; ++i) {
    dataUnits.push_back(new DataUnit(this, index));
    ++index;
  }
  return dataUnits;
}

const std::vector<std::string> CalcData::getURIValues() {
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

void CalcData::formatResponse() {
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

void CalcData::respond() {
  // format into httpResponse body
  this->formatResponse();
  // send to client
  this->httpResponse.send();
}

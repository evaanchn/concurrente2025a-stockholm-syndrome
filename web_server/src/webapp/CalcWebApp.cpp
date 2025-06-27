// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include <cassert>
#include <cstdint>
#include <sstream>
#include <vector>
#include <string>

#include "CalcWebApp.hpp"
#include "CalcData.hpp"
#include "DataUnit.hpp"
#include "WorkUnit.hpp"

bool CalcWebApp::canHandleHttpRequest(HttpRequest& httpRequest) {
  // If the request starts with appPrefix is for this web app
  if (httpRequest.getURI().rfind(appPrefix, 0) == 0) {
    return true;
  }
  // Unrecognized request
  return false;
}

void CalcWebApp::parseRequest(HttpRequest& httpRequest
    , std::vector<int64_t>& queries) {
  // Replace %xx hexadecimal codes by their ASCII symbols
  const std::string& uri = Util::decodeURI(httpRequest.getURI());
  // Numbers were asked in the form "/appPrefix/123,45,-7899" or
  // "/[appPrefix]?number=13"
  // Determine the position where numbers start
  // Find the position of '=' in the URI, if it exists
  // This is the start of the query string, e.g: "?number=123,456"
  // If no '=' is found, we assume the numbers start after the last '/'
  size_t numbersStart = uri.find('=');  // Start after '='
  if (numbersStart == std::string::npos) {
    // start after last '/'
    numbersStart = uri.rfind('/');
  }
  ++numbersStart;  // Move to the next character after '=' or '/'
  // TODO(you): Use arbitrary precision for numbers larger than int64_t
  const std::vector<std::string>& texts =
      Util::split(uri.substr(numbersStart), ",", true);
  // convert each query text into number
  for (size_t index = 0; index < texts.size(); ++index) {
    try {
      // Convert the text to a number. Provide an error message if not
      size_t end = 0;
      const int64_t value = std::stoll(texts[index], &end);
      if (end != texts[index].length()) {
        throw std::runtime_error("invalid number " + texts[index]);
      }
      queries.push_back(value);
    } catch (const std::exception& exception) {
      // ignore invalid imputs
      continue;
    }
  }
}

std::string CalcWebApp::serializeRequest(DataUnit* dataUnit) {
  assert(dataUnit);
  // Obtain original pointer to concurrent data
  uintptr_t originalDataPtr = reinterpret_cast<std::uintptr_t>
    (dataUnit->concurrentData);

  std::stringstream requestBuffer;  // Buffer for request's data
  // Write results in buffer
  requestBuffer << dataUnit->concurrentData->getAppIndex() <<
    '\n' << originalDataPtr << '\n' <<
    dataUnit->resultIndex << '\n' <<
    dataUnit->concurrentData->serializeQuery(dataUnit->resultIndex);

  return requestBuffer.str();
}

WorkUnit* CalcWebApp::deserializeRequest(std::string requestData) {
  assert(!requestData.empty());
  std::stringstream requestStream(requestData);
  size_t appIndex = 0;
  uintptr_t originalDataPtr = 0;
  size_t originalResultIdx = 0;
  int64_t query = 0;
  // Obtain appIndex, original concurrent data ptr and results index, with query
  if (!(requestStream >> appIndex >> originalDataPtr >> originalResultIdx
      >> query)) {
    throw std::runtime_error("Invalid request format");
  }
  // Classes that inherit from this one would know what type conc data to create
  return this->createWorkUnit(appIndex, originalDataPtr, originalResultIdx,
      query);
}

std::string CalcWebApp::serializeResponse(WorkUnit* workUnit) {
  assert(workUnit);
  // Obtain original pointer to concurrent data
  uintptr_t originalDataPtr = reinterpret_cast<std::uintptr_t>
    (workUnit->originalConcurrentData);

  std::stringstream responseData;  // Buffer for response's data
  responseData << workUnit->concurrentData->getAppIndex() <<
    '\n' << originalDataPtr << '\n' <<
    workUnit->originalResultIndex << '\n' <<
    workUnit->concurrentData->serializeResult(workUnit->resultIndex);

  return responseData.str();
}

DataUnit* CalcWebApp::deserializeResponse(std::string responseData) {
  assert(!responseData.empty());
  // CalcData and resultIndex
  std::stringstream responseStream(responseData);
  uintptr_t originalDataPtr = 0;
  size_t resultIndex = 0;
  if (!(responseStream >> originalDataPtr >> resultIndex)) {
    throw std::runtime_error("Invalid response format");
  }
  // Save result into the original CalcData object
  std::string queryResult;
  std::getline(responseStream, queryResult);
  reinterpret_cast<CalcData*>(originalDataPtr)->deserializeResult
    (resultIndex, queryResult);
  // Create a new DataUnit with the original data pointer and result index
  DataUnit* dataUnit = new DataUnit(reinterpret_cast<CalcData*>(originalDataPtr)
    , resultIndex);
  return dataUnit;
}

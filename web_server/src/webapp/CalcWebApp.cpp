// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include <cassert>
#include <cstdint>
#include <sstream>
#include <vector>
#include <string>

#include "common.hpp"
#include "CalcWebApp.hpp"
#include "CalcData.hpp"
#include "DataUnit.hpp"
#include "Log.hpp"
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

// Converts a DataUnit into a string format for network transmission
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
    dataUnit->concurrentData->serializeQuery(dataUnit->resultIndex) << '\n';

  return requestBuffer.str();
}

// Reconstructs a DataUnit from serialized string data
DataUnit* CalcWebApp::deserializeRequest(std::string requestData) {
  assert(!requestData.empty());
  std::stringstream requestStream(requestData);
  size_t appIndex = 0;
  uintptr_t originalDataPtr = 0;
  size_t originalResultIdx = 0;
  int64_t query = 0;
  // Split the result string by spaces
  const std::vector<std::string>& request =
    Util::split(requestData, "\n", true);
  // Validate minimum required fields are present
  if (request.size() < REQUEST_BUFFER_LINES_COUNT + 1) {
    throw std::runtime_error("Invalid request format " + requestData);
  }
  try {
    // Parse each component with type conversion:
    appIndex = std::stoul(request[0]);  // Which app should handle this
    originalDataPtr = std::stoul(request[1]);  // Original data location
    if (!originalDataPtr) {  // Null pointer check
      throw std::runtime_error("invalid data address");
    }
    originalResultIdx = std::stoul(request[2]);  // Position in results
    query = std::stoll(request[3]);  // Actual number to process
  } catch (const std::exception& exception) {
    throw std::runtime_error("Invalid serial CalcData: " + requestData);
  }
  // Classes that inherit from this one would know what type conc data to create
  Log::append(Log::INFO, "CalcWebApp",
    "Processig query: " + std::to_string(query));
  return this->createWorkUnit(appIndex, originalDataPtr, originalResultIdx,
      query);
}

DataUnit* CalcWebApp::deserializeResponse(std::string responseData) {
  assert(!responseData.empty());
  // CalcData and resultIndex
  uintptr_t originalDataPtr = 0;
  size_t resultIndex = 0;
  // Split the result string by spaces
  const std::vector<std::string>& response =
    Util::split(responseData, "\n", false);
  if (response.size() < RESPONSE_BUFFER_LINES_COUNT) {
    throw std::runtime_error("Invalid response format " + responseData);
  }
  try {
    // Parse the original data pointer (memory address as integer)
    originalDataPtr = std::stoul(response[0]);
    if (!originalDataPtr) {
      throw std::runtime_error("invalid data address");
    }
    // Get which result position this data belongs to
    resultIndex = std::stoul(response[1]);
  } catch (const std::exception& exception) {
    throw std::runtime_error("Invalid serial CalcData: " + responseData);
  }
  // Save result into the original CalcData object
  std::string queryResult = response[2].c_str();
  // Store the result in the original CalcData object
  reinterpret_cast<CalcData*>(originalDataPtr)->deserializeResult
    (resultIndex, queryResult);
  // Create a new DataUnit with the original data pointer and result index
  DataUnit* dataUnit = new DataUnit(reinterpret_cast<CalcData*>(originalDataPtr)
    , resultIndex);
  return dataUnit;
}

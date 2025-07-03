// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "ConcurrentData.hpp"

// Constructor for complete HTTP request/response pair
ConcurrentData::ConcurrentData(const HttpRequest& httpRequest,
  const HttpResponse& httpResponse, const size_t appIndex) :
  pendingQueries(0),  // Initialize with no pending queries
  httpRequest(httpRequest),  // Store client request
  httpResponse(httpResponse),  // Store response handler
  appIndex(appIndex) {  // Remember which app created this
}

// Constructor for placeholder data unit (used for stop conditions)
ConcurrentData::ConcurrentData(const size_t appIndex):
  pendingQueries(0),  // Initialize with no pending queries
  httpRequest(Socket()),  // Dummy empty request
  httpResponse(Socket(), ""),  // Dummy empty response
  appIndex(appIndex) {  // Remember which app created this
}

// Returns the index of the application that created this data unit
size_t ConcurrentData::getAppIndex() const {
    return this->appIndex;
}

// Marks one query as completed (thread-safe decrement)
void ConcurrentData::markUnitReady() {
  if (this->pendingQueries > 0) --this->pendingQueries;
}

// Checks if all queries have been processed
bool ConcurrentData::isReady() const {
  return this->pendingQueries == 0;
}

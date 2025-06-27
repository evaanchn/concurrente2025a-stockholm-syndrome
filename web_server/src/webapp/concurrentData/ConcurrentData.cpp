// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "ConcurrentData.hpp"

ConcurrentData::ConcurrentData(const HttpRequest& httpRequest,
  const HttpResponse& httpResponse, const size_t appIndex) :
  pendingQueries(0),
  httpRequest(httpRequest),
  httpResponse(httpResponse),
  appIndex(appIndex) {
}

ConcurrentData::ConcurrentData(const size_t appIndex):
  appIndex(appIndex),
  httpRequest(Socket()),
  httpResponse(Socket(), "") {
}

size_t ConcurrentData::getAppIndex() const {
    return this->appIndex;
}

void ConcurrentData::markUnitReady() {
  if (this->pendingQueries > 0) --this->pendingQueries;
}

bool ConcurrentData::isReady() const {
  return this->pendingQueries == 0;
}

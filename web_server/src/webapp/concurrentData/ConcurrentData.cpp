// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "ConcurrentData.hpp"

ConcurrentData::ConcurrentData(const HttpRequest& httpRequest
  , const HttpResponse& httpResponse)
  : pendingQueries(0)
  , httpRequest(httpRequest)
  , httpResponse(httpResponse) {
}

void ConcurrentData::markUnitReady() {
  if (pendingQueries > 0) --pendingQueries;
}

bool ConcurrentData::isReady() const {
  return (pendingQueries == 0);
}

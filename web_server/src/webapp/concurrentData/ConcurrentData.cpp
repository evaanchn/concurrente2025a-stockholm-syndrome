// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "ConcurrentData.hpp"

ConcurrentData::ConcurrentData(const HttpRequest& httpRequest
  , const HttpResponse& httpResponse)
  : pendingQueries(0)
  , httpRequest(httpRequest)
  , httpResponse(httpResponse) {
}

void ConcurrentData::markUnitReady() {
  if (this->pendingQueries > 0) --this->pendingQueries;
}

bool ConcurrentData::isReady() const {
  return this->pendingQueries == 0;
}

// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "RequestData.hpp"

RequestData::RequestData(const HttpRequest& httpRequest
  , const HttpResponse& httpResponse)
  : pendingQueries(0)
  , httpRequest(httpRequest)
  , httpResponse(httpResponse) {
}

void RequestData::markUnitReady() {
  if (pendingQueries > 0) --pendingQueries;
}

bool RequestData::isReady() const {
  return (pendingQueries == 0);
}

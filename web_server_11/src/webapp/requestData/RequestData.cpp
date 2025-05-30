// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "RequestData.hpp"

RequestData::RequestData(HttpRequest& httpRequest)
  : pendingQueries(0)
  , httpResponse(httpRequest.getSocket(), httpRequest.getHttpVersion()) {
}

HttpResponse& RequestData::getHttpResponse() {
  return this->httpResponse;
}

void RequestData::signalUnitReady() {
  if (pendingQueries > 0) --pendingQueries;
}

bool RequestData::isReady() {
  return (pendingQueries == 0);
}

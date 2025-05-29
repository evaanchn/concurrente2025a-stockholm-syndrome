// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "RequestData.hpp"

RequestData::RequestData(HttpRequest& httpRequest, ConcurrentApp* concurrentApp)
: pendingQueries(0)
, httpResponse(httpRequest.getSocket(), httpRequest.getHttpVersion())
, concurrentApp(concurrentApp) {
}

HttpResponse& RequestData::getHttpResponse() {
  return this->httpResponse;
}

ConcurrentApp* RequestData::getConcurrentApp() {   
  return this->concurrentApp;
}

inline void RequestData::signalUnitReady() {
  if (pendingQueries > 0) --pendingQueries;
}

inline bool RequestData::isReady() {
  return (pendingQueries == 0);
}

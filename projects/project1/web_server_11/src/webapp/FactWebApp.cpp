// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "FactWebApp.hpp"

FactWebApp::FactWebApp()
: CalcWebApp("/fact", "/fact?number=", "Prime Factoritation") {
}

void FactWebApp::buildResponse(const int64_t value
  , HttpResponse& httpResponse) {
}

void FactWebApp::factResponse(std::vector<int64_t>& facts
  , HttpResponse& httpResponse) {
}

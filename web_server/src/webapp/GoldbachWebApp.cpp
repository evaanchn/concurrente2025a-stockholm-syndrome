// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "GoldbachWebApp.hpp"
#include "GoldbachData.hpp"
#include "HomeWebApp.hpp"
#include "WorkUnit.hpp"

GoldbachWebApp::GoldbachWebApp()
: CalcWebApp("/goldbach", "/goldbach?number=", "Goldbach sums") {
}

ConcurrentData* GoldbachWebApp::createConcurrentData(HttpRequest& httpRequest
    , HttpResponse& httpResponse, const size_t appIndex) {
  // Serve header of the html
  HomeWebApp::serveHeader(httpResponse, this->title);
  // Create a GoldbachData object to store concurrent data
  GoldbachData* data = new GoldbachData(httpRequest, httpResponse, appIndex);
  if (data) {
    // Parse the request to extract queries
    this->parseRequest(httpRequest, data->getQueries());
    // If no queries were parsed, send response and delete data
    if (data->getQueries().empty()) {
      data->respond();
      // deallocate data
      delete data;
      return nullptr;
    }
    data->updatePending();
  }
  return data;
}

DataUnit* GoldbachWebApp::createWorkUnit(size_t appIndex,
    uintptr_t originalDataPtr, size_t originalResultIdx, int64_t query) {
  // Create a PrimeFactData object to store data
  GoldbachData* goldbachData = new GoldbachData(appIndex);
  // Add singular query to data
  goldbachData->getQueries().push_back(query);
  // Work unit has usable new data created in worker's machine (index 0 for
  // query since there will only be one), and stores original data and index
  WorkUnit* workUnit = new WorkUnit(goldbachData, /*resultIndex*/ 0,
      reinterpret_cast<CalcData*>(originalDataPtr), originalResultIdx);
  return workUnit;
}

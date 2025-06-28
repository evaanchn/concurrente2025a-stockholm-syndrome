// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef GOLDBACHWEBAPP_HPP
#define GOLDBACHWEBAPP_HPP

#include <string>

#include "CalcWebApp.hpp"

/**
@brief A web application that calculates Goldbach sums
*/
class GoldbachWebApp : public CalcWebApp {
  /// Objects of this class cannot be copied
  DISABLE_COPY(GoldbachWebApp);

 public:
  /// Constructor
  GoldbachWebApp();
  /// Destructor
  ~GoldbachWebApp() = default;

 public:
  /// @brief Create a ConcurrentData object to store data
  /// @param httpRequest is the request to be handled
  /// @param httpResponse is the response to be handled
  /// @param appIndex index of the application that created this data
  /// @return a pointer to the created ConcurrentData object
  ConcurrentData* createConcurrentData(HttpRequest& httpRequest
    , HttpResponse& httpResponse, const size_t appIndex) override;

 protected:
  /// @brief Creates a work unit with corresponding ConcurrentData type.
  /// @param appIndex Index of concurrent app that creates the worker unit.
  /// @param originalDataPtr Pointer to concurrent data that works in master.
  /// @param originalResultIdx Index of result that works in master.
  /// @param query The number to process.
  /// @return The work unit created from the paramters.
  /// @note Used in worker process.
  WorkUnit* createWorkUnit(size_t appIndex, uintptr_t originalDataPtr,
      size_t originalResultIdx, int64_t query) override;
};

#endif  // GOLDBACHWEBAPP_HPP

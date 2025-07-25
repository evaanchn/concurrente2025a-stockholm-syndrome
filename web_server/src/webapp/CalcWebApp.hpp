// Copyright 2021,2024 Jeisson Hidalgo-Cespedes. ECCI-UCR. CC BY 4.0
// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef CALCWEBAPP_HPP
#define CALCWEBAPP_HPP

#include <string>
#include <vector>

#include "ConcurrentApp.hpp"
#include "HttpApp.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Util.hpp"

// forward declaration
struct DataUnit;

/// @brief CalcWebApp is a base class for web applications that handle
class CalcWebApp : public ConcurrentApp {
  /// Objects of this class cannot be copied
  DISABLE_COPY(CalcWebApp);

 protected:
  // URI prefix to identify each app
  const std::string appPrefix;
  // URI prefix for listing values
  const std::string valuesPrefix;
  // HTML page title
  const std::string title;

 public:
  /// Constructor
  CalcWebApp(const std::string appPrefix, const std::string valuesPrefix
    , std::string title)
  : appPrefix(appPrefix)
  , valuesPrefix(valuesPrefix)
  , title(title) {
  }

  /// Called by the web server when the web server is started
  void start() override {
  }

  /// Called when the web server stops, in order to allow the web application
  /// clean up and finish as well
  void stop() override {
  }
  /// Check if this application can handle the HTTP request
  /// @return true If this application handled the request, false otherwise
  /// and another chained application should handle it
  bool canHandleHttpRequest(HttpRequest& httpRequest) override;

 public:
  /// @brief Parse the HTTP request to parse numbers from the URI
  /// @param httpRequest request to be parsed
  /// @param queries is the vector to store the parsed numbers
  void parseRequest(HttpRequest& httpRequest, std::vector<int64_t>& queries);


 public:  // distribution
  /// @brief Create a plain text from DataUnit to be send into the network
  /// @param dataUnit data to be parsed into text
  /// @return parsed text
  std::string serializeRequest(DataUnit* dataUnit) override;
  /// @brief Parse received response, saving the results and creating its
  /// corresponding WorkUnit
  /// @param requestData text to be parsed into a WorkUnit
  /// @return pointer to the created WorkUnit
  DataUnit* deserializeRequest(std::string requestData) override;
  /// @brief Parse received response, saving the results and creating its
  /// corresponding DataUnit
  /// @param responseData text to be parsed into a DataUnit
  /// @return pointer to the created DataUnit
  DataUnit* deserializeResponse(std::string responseData) override;

 protected:
  /// @brief Creates a work unit with corresponding ConcurrentData type.
  /// @param appIndex Index of concurrent app that creates the worker unit.
  /// @param originalDataPtr Pointer to concurrent data that works in master.
  /// @param originalResultIdx Index of result that works in master.
  /// @param query The number to process.
  /// @return The work unit created from the paramters.
  /// @note Used in worker process.
  virtual DataUnit* createWorkUnit(size_t appIndex, uintptr_t originalDataPtr,
      size_t originalResultIdx, int64_t query) = 0;
};

#endif  // CALCWEBAPP_HPP

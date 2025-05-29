// Copyright 2021,2024 Jeisson Hidalgo-Cespedes. ECCI-UCR. CC BY 4.0
// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef CALCWEBAPP_HPP
#define CALCWEBAPP_HPP

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "ConcurrentApp.hpp"
#include "HomeWebApp.hpp"
#include "HttpApp.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Util.hpp"

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
  bool canHandleHttpRequest(HttpRequest& httpRequest,
    HttpResponse& httpResponse) override;

 public:
  /// @brief Parse the HTTP request to parse numbers from the URI
  /// @param httpRequest request to be parsed
  /// @param httpResponse response to the client
  /// @param queries is the vector to store the parsed numbers
  void parseRequest(HttpRequest& httpRequest, HttpResponse& httpResponse
    , std::vector<int64_t>& queries);
  /// @brief Format the response with the results of the request
  /// @details This method is called by the web server to format the response
  /// with the results of the request. It builds the HTML response body
  /// with the results of the calculation.
  /// @param results is the vector of results to be formatted
  /// @param httpResponse is the response to be sent back to the client
  /// @remark The results vector contains a vector of int64_t for each
  /// calculation, where the first element is the original value and the
  /// rest of the elements are the results of the calculation.
  void formatResponse(std::vector<std::vector<int64_t>>& results,
      HttpResponse& httpResponse);
  /// @brief Unary calculation response for a given value to be implemented by
  // the subclases
  /// @param result vector containing the original value and its result elements
  /// @param httpResponse The object to answer to the client/user
  virtual void buildResult(std::vector<int64_t>& result) = 0;

};

#endif  // CALCWEBAPP_HPP
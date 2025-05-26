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

#include "HomeWebApp.hpp"
#include "HttpApp.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Util.hpp"

// TODO(us) Add intermediary class for concurrent app

/**
@brief A web application generic template for unary calculation
*/
template <typename ValueType>
class CalcWebApp : public HttpApp {
 protected:
  // URI prefix to identify each app
  const std::string appPrefix;
  // URI prefix for listing values
  const std::string valuesPrefix;
  // HTML page title
  const std::string title;

  /// Objects of this class cannot be copied
  DISABLE_COPY(CalcWebApp);

 public:
  /// Constructor
  CalcWebApp(const std::string appPrefix, const std::string valuesPrefix
    , std::string title)
  : appPrefix(appPrefix)
  , valuesPrefix(valuesPrefix)
  , title(title) {
  }

  /// Destructor
  ~CalcWebApp() {
  }

  /// Called by the web server when the web server is started
  void start() override {
  }

  /// Handle HTTP requests. @see HttpServer::handleHttpRequest()
  /// @return true If this application handled the request, false otherwise
  /// and another chained application should handle it
  bool handleHttpRequest(HttpRequest& httpRequest,
      HttpResponse& httpResponse) override {
    // If the request starts with appPrefix is for this web app
    if (httpRequest.getURI().rfind(appPrefix, 0) == 0) {
      return this->serveCalculation(httpRequest, httpResponse);
    }
    // Unrecognized request
    return false;
  }

  /// Called when the web server stops, in order to allow the web application
  /// clean up and finish as well
  void stop() override {
  }

 protected:
  /// Handle a HTTP request that starts with "/fact"
  /// @return true if the calculation was handled, false if it must be
  /// handled by another application
  bool serveCalculation(HttpRequest& httpRequest, HttpResponse& httpResponse) {
    // Replace %xx hexadecimal codes by their ASCII symbols
    const std::string& uri = Util::decodeURI(httpRequest.getURI());
    // Build the body of the response
    // HomeWebApp::serveHeader(httpResponse, this->title);
    HomeWebApp::serveHeader(httpResponse, this->title);
    httpResponse.body()
      // << "  <p>request: " << uri << "</p>\n"
      << "  <ol type=""A"">\n";
    this->analyzeValueList(uri, httpResponse);
    httpResponse.body()
      << "  </ol>\n"
      << "</html>\n";
    // Send the response to the client (user agent)
    return httpResponse.send();
  }

  /// @brief Extract comma-separated values to realize them an unary
  /// calculation, from uri and add their results to the HTTP response
  /// @param uri request URI in form "/valuesPrefixx1,x2,...,xn" or
  /// "/valuesPrefixx1,x2,...,xn"
  /// @param httpResponse The object to answer to the client/user
  void analyzeValueList(const std::string& uri, HttpResponse& httpResponse) {
    // Numbers were asked in the form "/appPrefix/123,45,-7899" or
    // "/[appPrefix]?number=13"
    // Determine the position where numbers start
    size_t numbersStart = appPrefix.length();
    if (uri.rfind(valuesPrefix, 0) == 0) {
      numbersStart = valuesPrefix.length();
    }
    // TODO(you): Use arbitrary precision for numbers larger than int64_t
    const std::vector<std::string>& texts =
        Util::split(uri.substr(numbersStart), ",", true);
    // For each asked number, provide its calculation
    for (size_t index = 0; index < texts.size(); ++index) {
      try {
        // Convert the text to a number. Provide an error message if not
        size_t end = 0;
        const ValueType value = std::stoll(texts[index], &end);
        if (end != texts[index].length()) {
          throw std::runtime_error("invalid number " + texts[index]);
        }
        this->buildResponse(value, httpResponse);
      } catch (const std::exception& exception) {
        // Text was not a valid number, report an error to user
        httpResponse.body() << "    <li class=err>" << texts[index]
            << ": invalid number\n";
      }
    }
  }

  /// @brief Unary calculation response for a given value to be implemented by
  // the subclases
  /// @param value Data required for the calculation
  /// @param httpResponse The object to answer to the client/user
  virtual void buildResponse(const ValueType value, HttpResponse& httpResponse)
    = 0;
};

#endif  // CALCWEBAPP_HPP

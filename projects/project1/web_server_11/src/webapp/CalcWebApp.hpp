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

/**
@brief A web application generic template for unary calculation
*/
template <typename ValueType>
class CalcWebApp : public HttpApp {
  /// Objects of this class cannot be copied
  DISABLE_COPY(CalcWebApp);

 public:
  /// Constructor
  CalcWebApp(const char* const prefix1, const char* const prefix2
    , std::string title)
  : PREFIX1(prefix1)
  , PREFIX2(prefix2)
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
    // If the request starts with PREFIX1 is for this web app
    if (httpRequest.getURI().rfind(PREFIX1, 0) == 0) {
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
      << "  <p>request: " << uri << "</p>\n"
      << "  <ol type=""A"">\n";
    this->analyzeValueList(uri, httpResponse);
    httpResponse.body()
      << "  </ol>\n"
      << "  <hr>\n"
      << "  <p><a href=\"/\">Back</a></p>\n"
      << "</html>\n";
    // Send the response to the client (user agent)
    return httpResponse.send();
  }

  /// @brief Extract comma-separated values to realize them an unary
  /// calculation, from uri and add their results to the HTTP response
  /// @param uri request URI in form "/PREFIX1=x1,x2,...,xn" or
  /// "/PREFIX2=x1,x2,...,xn"
  /// @param httpResponse The object to answer to the client/user
  void analyzeValueList(const std::string& uri, HttpResponse& httpResponse) {
    // Numbers were asked in the form "/PREFIX1/123,45,-7899" or
    // "/[PREFIX]?number=13"
    // Determine the position where numbers start
    size_t numbersStart = std::strlen(PREFIX1);
    if (uri.rfind(PREFIX2, 0) == 0) {
      numbersStart = std::strlen(PREFIX2);
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

 protected:
  const char* const PREFIX1;
  const char* const PREFIX2;
  const std::string title;
};

#endif  // CALCWEBAPP_HPP

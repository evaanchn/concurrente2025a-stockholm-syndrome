// Copyright 2021,2024 Jeisson Hidalgo-Cespedes. ECCI-UCR. CC BY 4.0

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "FactWebApp.hpp"
#include "GoldbachWebApp.hpp"
#include "HomeWebApp.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Util.hpp"

HomeWebApp::HomeWebApp() {
}

HomeWebApp::~HomeWebApp() {
}

void HomeWebApp::start() {
}

void HomeWebApp::stop() {
}

bool HomeWebApp::handleHttpRequest(HttpRequest& httpRequest,
    HttpResponse& httpResponse) {
  // If the home page was asked
  // TODO(you): Homepage should be served by a HomeWebApp object
  if (httpRequest.getMethod() == "GET" && httpRequest.getURI() == "/") {
    return this->serveHomepage(httpRequest, httpResponse);
  }
  return false;
}

bool HomeWebApp::serveHomepage(HttpRequest& httpRequest
  , HttpResponse& httpResponse) {
  (void)httpRequest;
  // TODO(you): Homepage should be served by a HomeWebApp object
  // Build the body of the response
  this->serveHeader(httpResponse, "Calculator Server");
  httpResponse.body()
    // Prime Factorization form
    << "  <form method=get action=" << FACT_PREFIX << ">\n"
    << "    <label for=number>Number</label>\n"
    << "    <input type=text id=number name=number "
              "pattern=\"\\[+\\-]?d+(,[+\\-]?\\d+)*\" required>\n"
    << "    <button type=submit>Factorize</button>\n"
    << "  </form>\n"
    << "  <br>"
    // Goldbach Sums form
    << "  <form method=get action=" << GOLDBACH_PREFIX << ">\n"
    << "    <label for=number>Number</label>\n"
    << "    <input type=text id=number name=number "
              "pattern=\"\\[+\\-]?d+(,[+\\-]?\\d+)*\" required>\n"
    << "    <button type=submit>Goldbach Sums</button>\n"
    << "  </form>\n"
    << "</html>\n";
  // Send the response to the client (user agent)
  return httpResponse.send();
}

void HomeWebApp::serveHeader(HttpResponse& httpResponse,
    const std::string& title) {
  // Set HTTP response metadata (headers)
  httpResponse.setHeader("Server", "AttoServer v1.0");
  httpResponse.setHeader("Content-type", "text/html; charset=ascii");
  // Serve document header and title
  httpResponse.body() << "<!DOCTYPE html>\n"
    << "<html lang=en>\n"
    << "  <meta charset=utf-8>\n"
    << "  <title>" << title << "</title>\n"
    << "  <style>body {font-family: monospace} .err {color: red}</style>\n"
    << "  <h1>" << title << "</h1>\n";
}

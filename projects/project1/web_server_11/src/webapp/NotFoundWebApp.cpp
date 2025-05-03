// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#include "NotFoundWebApp.hpp"

bool NotFoundWebApp::handleHttpRequest(HttpRequest& httpRequest,
    HttpResponse& httpResponse) {
  return this->serveNotFound(httpRequest, httpResponse);
}

bool NotFoundWebApp::serveNotFound(HttpRequest& httpRequest
    , HttpResponse& httpResponse) {
    (void)httpRequest;
  // TODO(you): 404 not found page should be served by a NotFoundWebApp object
  // Set HTTP response metadata (headers)
  httpResponse.setStatusCode(404);
  httpResponse.setHeader("Server", "AttoServer v1.0");
  httpResponse.setHeader("Content-type", "text/html; charset=ascii");

  // Build the body of the response
  std::string title = "Not found";
  httpResponse.body() << "<!DOCTYPE html>\n"
      << "<html lang=\"en\">\n"
      << "  <meta charset=\"ascii\"/>\n"
      << "  <title>" << title << "</title>\n"
      << "  <style>body {font-family: monospace} h1 {color: red}</style>\n"
      << "  <h1>" << title << "</h1>\n"
      << "  <p>The requested resource was not found on this server.</p>\n"
      << "  <hr><p><a href=\"/\">Homepage</a></p>\n"
      << "</html>\n";

  // Send the response to the client (user agent)
  return httpResponse.send();
}

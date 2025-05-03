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
  this->serveHeader(httpResponse, "Calculator Server");

  httpResponse.body()
    << "    <div class='form-container'>\n"
    // Prime Factorization form
    << "      <div class='form-group'>\n"
    << "        <form method='get' action='" << FACT_PREFIX << "'>\n"
    << "          <label for='number'>Prime Factorization</label>\n"
    << "          <input type='text' id='number' name='number' "
              "pattern=\"[+\\-]?\\d+(\\s*,\\s*[+\\-]?\\d+)*\" "
              "placeholder='Ej: 17, 22, 33' required>\n"
    << "          <button type='submit'>Factorize</button>\n"
    << "        </form>\n"
    << "      </div>\n"
    // Goldbach Sums form
    << "      <div class='form-group'>\n"
    << "        <form method='get' action='" << GOLDBACH_PREFIX << "'>\n"
    << "          <label for='number'>Goldbach Sums</label>\n"
    << "          <input type='text' id='number' name='number' "
              "pattern=\"[+\\-]?\\d+(\\s*,\\s*[+\\-]?\\d+)*\" "
              "placeholder='Ej: 33, 44, -142' required>\n"
    << "          <button type='submit'>Calculate Sums</button>\n"
    << "        </form>\n"
    << "      </div>\n"
    << "    </div>\n"
    << "  </div>\n"
    << "</body>\n"
    << "</html>\n";

  return httpResponse.send();
}

void HomeWebApp::serveHeader(HttpResponse& httpResponse,
  const std::string& title) {
// Set HTTP response metadata (headers)
httpResponse.setHeader("Server", "AttoServer v1.2");
httpResponse.setHeader("Content-type", "text/html; charset=utf-8");

// Serve document header with modern design
httpResponse.body() << "<!DOCTYPE html>\n"
  << "<html lang='en'>\n"
  << "<head>\n"
  << "  <meta charset='utf-8'>\n"
  << "  <meta name='viewport' content='width=device-width, initial-scale=1'>\n"
  << "  <title>" << title << "</title>\n"
  << "  <style>\n"
  << "    :root {\n"
  << "      --primary: #4361ee;\n"
  << "      --secondary: #3a0ca3;\n"
  << "      --accent: #f72585;\n"
  << "      --light: #f8f9fa;\n"
  << "      --dark: #212529;\n"
  << "      --success: #4cc9f0;\n"
  << "      --error: #ef233c;\n"
  << "      --border: #dee2e6;\n"
  << "      --background: #f5f7ff;\n"
  << "    }\n"
  << "    * {\n"
  << "      box-sizing: border-box;\n"
  << "      margin: 0;\n"
  << "      padding: 0;\n"
  << "    }\n"
  << "    body {\n"
  << "      font-family: 'Segoe UI', Roboto, 'Helvetica Neue', sans-serif;\n"
  << "      line-height: 1.6;\n"
  << "      color: var(--dark);\n"
  << "      background-color: var(--background);\n"
  << "      padding: 20px;\n"
  << "    }\n"
  << "    .container {\n"
  << "      max-width: 800px;\n"
  << "      margin: 0 auto;\n"
  << "      background: white;\n"
  << "      border-radius: 10px;\n"
  << "      box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);\n"
  << "      padding: 30px;\n"
  << "    }\n"
  << "    h1 {\n"
  << "      color: var(--secondary);\n"
  << "      text-align: center;\n"
  << "      margin-bottom: 30px;\n"
  << "      font-weight: 600;\n"
  << "      border-bottom: 2px solid var(--primary);\n"
  << "      padding-bottom: 10px;\n"
  << "    }\n"
  << "    .form-container {\n"
  << "      display: grid;\n"
  << "      grid-template-columns: 1fr 1fr;\n"
  << "      gap: 20px;\n"
  << "      margin-bottom: 30px;\n"
  << "    }\n"
  << "    .form-group {\n"
  << "      margin-bottom: 0;\n"
  << "    }\n"
  << "    label {\n"
  << "      display: block;\n"
  << "      margin-bottom: 8px;\n"
  << "      font-weight: 500;\n"
  << "      color: var(--secondary);\n"
  << "    }\n"
  << "    input[type='text'] {\n"
  << "      width: 100%;\n"
  << "      padding: 12px;\n"
  << "      border: 2px solid var(--border);\n"
  << "      border-radius: 6px;\n"
  << "      font-size: 16px;\n"
  << "      transition: border 0.3s;\n"
  << "      margin-bottom: 10px;\n"
  << "    }\n"
  << "    input[type='text']:focus {\n"
  << "      border-color: var(--primary);\n"
  << "      outline: none;\n"
  << "    }\n"
  << "    button {\n"
  << "      background-color: var(--primary);\n"
  << "      color: white;\n"
  << "      border: none;\n"
  << "      padding: 12px 20px;\n"
  << "      border-radius: 6px;\n"
  << "      cursor: pointer;\n"
  << "      font-size: 16px;\n"
  << "      font-weight: 500;\n"
  << "      transition: all 0.3s;\n"
  << "      width: 100%;\n"
  << "    }\n"
  << "    button:hover {\n"
  << "      background-color: var(--secondary);\n"
  << "      transform: translateY(-2px);\n"
  << "    }\n"
  << "    .help-text {\n"
  << "      text-align: center;\n"
  << "      margin-top: 20px;\n"
  << "      color: #666;\n"
  << "      font-size: 14px;\n"
  << "      line-height: 1.5;\n"
  << "    }\n"
  << "    .results-container {\n"
  << "      background: white;\n"
  << "      border-radius: 8px;\n"
  << "      box-shadow: 0 2px 4px rgba(0,0,0,0.1);\n"
  << "      padding: 25px;\n"
  << "    .result-item {\n"
  << "      padding: 15px 0;\n"
  << "    }\n"
  << "    .number-result {\n"
  << "      font-size: 1.2rem;\n"
  << "      margin-bottom: 10px;\n"
  << "    }\n"
  << "    .number-value {\n"
  << "      font-weight: bold;\n"
  << "      color: var(--primary);\n"
  << "    }\n"
  << "    .sums-count {\n"
  << "      color: var(--accent);\n"
  << "      font-weight: bold;\n"
  << "    }\n"
  << "    .sums-grid {\n"
  << "      display: grid;\n"
  << "      grid-template-columns: repeat(auto-fill, minmax(200px, 1fr));\n"
  << "      gap: 10px;\n"
  << "      margin-top: 15px;\n"
  << "    }\n"
  << "    .sum-item {\n"
  << "      background: #f0f4ff;\n"
  << "      padding: 8px 12px;\n"
  << "      border-radius: 6px;\n"
  << "      border-left: 3px solid var(--accent);\n"
  << "    }\n"
  << "    .factorization {\n"
  << "      font-size: 1.2rem;\n"
  << "      line-height: 1.8;\n"
  << "    }\n"
  << "    .factor {\n"
  << "      color: var(--primary);\n"
  << "      font-weight: bold;\n"
  << "    }\n"
  << "    .exponent {\n"
  << "      color: var(--accent);\n"
  << "      font-size: 0.8rem;\n"
  << "      position: relative;\n"
  << "      top: -0.6em;\n"
  << "      margin-left: 1px;\n"
  << "    }\n"
  << "    .err {\n"
  << "      color: var(--error);\n"
  << "    }\n"
  << "    .back-button {\n"
  << "      display: inline-block;\n"
  << "      margin-top: 20px;\n"
  << "      color: var(--primary);\n"
  << "      text-decoration: none;\n"
  << "      font-weight: 500;\n"
  << "      padding: 8px 16px;\n"
  << "      border: 1px solid var(--primary);\n"
  << "      border-radius: 4px;\n"
  << "      transition: all 0.3s;\n"
  << "    }\n"
  << "    .back-button:hover {\n"
  << "      background-color: var(--primary);\n"
  << "      color: white;\n"
  << "      text-decoration: none;\n"
  << "    }\n"
  << "    @media (max-width: 600px) {\n"
  << "      .form-container {\n"
  << "        grid-template-columns: 1fr;\n"
  << "      }\n"
  << "      .container {\n"
  << "        padding: 20px;\n"
  << "      }\n"
  << "      .sums-grid {\n"
  << "        grid-template-columns: 1fr;\n"
  << "      }\n"
  << "    }\n"
  << "  </style>\n"
  << "</head>\n"
  << "<body>\n"
  << "  <div class='container'>\n"
  << "    <h1>" << title << "</h1>\n";
}

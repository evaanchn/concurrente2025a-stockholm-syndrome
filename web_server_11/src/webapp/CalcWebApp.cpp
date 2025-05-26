// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "CalcWebApp.hpp"

bool CalcWebApp::canHandleHttpRequest(HttpRequest& httpRequest,
      HttpResponse& httpResponse) {
  // If the request starts with appPrefix is for this web app
  if (httpRequest.getURI().rfind(appPrefix, 0) == 0) {
    // Serve header of the html
    HomeWebApp::serveHeader(httpResponse, this->title);
    // start ordered list of results
    httpResponse.body()
      << "  <ol type=""A"">\n";
    return true;
  }
  // Unrecognized request
  return false;
}

void CalcWebApp::parseRequest(HttpRequest& httpRequest,
    HttpResponse& httpResponse, std::vector<int64_t>& query) {
  // Replace %xx hexadecimal codes by their ASCII symbols
  const std::string& uri = Util::decodeURI(httpRequest.getURI());
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
      const int64_t value = std::stoll(texts[index], &end);
      if (end != texts[index].length()) {
        throw std::runtime_error("invalid number " + texts[index]);
      }
      query.push_back(value);
    } catch (const std::exception& exception) {
      // Text was not a valid number, report an error to user
      httpResponse.body() << "    <li class=err>" << texts[index]
          << ": invalid number\n";
    }
  }
}

void CalcWebApp::formatResponse(
    std::vector<std::vector<int64_t>>& results, HttpResponse& httpResponse) {
  // For each result, build the response
  for (auto& result : results) {
    this->buildResult(result, httpResponse);
  }
  // End ordered list of results
  httpResponse.body()
    << "  </ol>\n"
    << "</html>\n";
}

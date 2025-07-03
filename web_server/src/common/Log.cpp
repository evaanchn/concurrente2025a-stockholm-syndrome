// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Log.hpp"

/// The respective texts for the message types
const char* const Log::MESSAGE_TYPE_TEXT[] = {
  "Debug",
  "Info",
  "Warning",
  "Error",
};

// Returns the singleton instance (created on first call)
Log& Log::getInstance() {
  static Log log;
  return log;
}

// Constructor initializes output stream with std::cout's buffer
Log::Log()
  : output{std::cout.rdbuf()} {
}

// Configures the log output destination (file or stdout)
void Log::start(const std::string& logFilename) {
  // If filename is empty, use stdout
  if (logFilename.empty()) {
    // Adapted from https://stackoverflow.com/a/366969
    this->output.rdbuf(std::cout.rdbuf());
    this->filename = "(stdout)";
  } else {
    // Ensure file isn't already open, then open in append mode
    assert(this->file.is_open() == false);
    this->file.open(logFilename, std::ios::app);
    if (!this->file) {
      throw std::runtime_error("could not open log file for appending: "
        + logFilename);
    }
    this->filename = logFilename;
    this->output.rdbuf(this->file.rdbuf());
  }
}

// Closes the log file if it was open
void Log::stop() {
  this->file.close();
}

// Thread-safe log message writing with error checking
void Log::write(Log::MessageType type, const std::string& category
  , const std::string& text) {
  this->mutex.lock();

  // Write message in TSV format: type, category, text
  this->output << MESSAGE_TYPE_TEXT[type]
    << '\t' << category
    << '\t' << text << std::endl;

  // Check for write errors before releasing lock
  bool error = !this->output;
  this->mutex.unlock();

  if (error) {
    throw std::runtime_error("could not write log file: " + this->filename);
  }
}

// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#ifndef COMMON_HPP
#define COMMON_HPP

/// Declare four of the rule-of-the-five methods
#define DECLARE_RULE4(Class, action) \
  Class(const Class& other) = action; \
  Class(Class&& other) = action; \
  Class& operator=(const Class& other) = action; \
  Class& operator=(Class&& other) = action

/// Disable default methods for copying objects
#define DISABLE_COPY(Class) \
  DECLARE_RULE4(Class, delete)

/// For data transmissions
typedef unsigned char byte;

// Web server constants

#define DISTRIBUTED true

#define DEFAULT_PORT "8080"

#define DEFAULT_MASTER_PORT "9090"

#define RESPONSE_BUFFER_LINES_COUNT 3

#define REQUEST_BUFFER_LINES_COUNT 3

#define DEFAULT_MAX_WORKER_CONNECTIONS 4

#define WORKER_PASSWORD "workerSecret"

#endif  // COMMON_HPP

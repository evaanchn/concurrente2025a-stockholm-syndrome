// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

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

// Serialized body data elements count
#define BODY_COLLISION_DATA_SIZE 8
#define BODY_ACCELERATION_DATA_SIZE 4
#define BODY_DISTANCE_DATA_SIZE 3
#define BODY_VELOCITY_DATA_SIZE 3

/// @brief Collision data indexes for serialized bodies
enum CollisionData{
  COLLISION_MASS = 0,
  COLLISION_RADIUS = 1,
  COLLISION_POSITION_X = 2,
  COLLISION_POSITION_Y = 3,
  COLLISION_POSITION_Z = 4,
  COLLISION_VELOCITY_X = 5,
  COLLISION_VELOCITY_Y = 6,
  COLLISION_VELOCITY_Z = 7
};

/// @brief Acceleration data indexes for serialized bodies
enum AccelerationData{
  ACCELERATION_MASS = 0,
  ACCELERATION_POSITION_X = 1,
  ACCELERATION_POSITION_Y = 2,
  ACCELERATION_POSITION_Z = 3
};

// Excution modes for the simulation
enum ExecutionMode {
  UNIVERSE_FILE_MODE, RANDOM_UNIVERSE_MODE
};

// Common arguments positions for indexing
enum CommonArgumentsPositions {
  DELTA_T = 2, MAX_TIME
};

#define UNIVERSE_FILE 1

// Random universe mode arguments positions for indexing
enum RandomUniverseModePositions {
  BODIES_COUNT = 1, MIN_MASS = 4, MAX_MASS, MIN_RADIUS, MAX_RADIUS,
  MIN_POS, MAX_POS, MIN_VEL, MAX_VEL
};

// Default values for random universe mode
#define DEFAULT_MIN_MASS 10.0
#define DEFAULT_MAX_MASS 2000.0
#define DEFAULT_MIN_RADIUS 1.0
#define DEFAULT_MAX_RADIUS 1000.0
#define DEFAULT_MIN_POS -1000.0
#define DEFAULT_MAX_POS 1000.0
#define DEFAULT_MIN_VEL -1000.0
#define DEFAULT_MAX_VEL 1000.0

// MPI tags
#define COLLISION_TAG 100
#define ACCELERATION_TAG 101

#endif  // COMMON_HPP

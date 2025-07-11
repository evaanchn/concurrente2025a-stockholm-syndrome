// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <string>
#include <vector>

#include"Body.hpp"
#include "common.hpp"
#include "RealVector.hpp"

// forward declaration
// class Mpi;

class Simulation {
 private:
  std::string universeFile;
  double deltaTime = 0.0;
  double maxTime = 0.0;

  double minMass = 0.0;
  double maxMass = 0.0;
  double minRadius = 0.0;
  double maxRadius = 0.0;
  double minPosition = 0.0;
  double maxPosition = 0.0;
  double minVelocity = 0.0;
  double maxVelocity = 0.0;

  size_t totalBodyCount = 0;
  size_t activeBodiesCount = 0;
  std::vector<Body> bodies;

 public:
  /// @brief Constructor for the Simulation class.
  Simulation() = default;
  DECLARE_RULE4(Simulation, delete);
  ~Simulation() = default;

  void run(int argc, char* argv[]);

 private:
  ExecutionMode analyzeArguments(int argc, char* argv[]);
  bool analyzeRandomUniverseModeArguments(int argc, char* argv[]);

 private:
  /// @brief Load the universe from a file.
  void loadUniverse();
  /// @brief create a random universe.
  void createUniverse();
  /// @brief Check the collisions between bodies in the simulation.
  void checkCollisions();
  /// @brief Check the collisions between bodies in the simulation.
  /// @param serialBodies A vector containing the mass, radius, position, and
  /// velocity of other bodie.
  void checkCollisions(std::vector<double>& serialBodies);
  /// @brief Update the accelerations of the bodies in the simulation.
  void updateAccelerations();
  /// @brief Update the accelerations of the bodies in the simulation.
  /// @param serialBodies A vector containing the mass and position
  // of other bodies.
  void updateAccelerations(std::vector<double>& serialBodies);
  /// @brief Update the velocities of the bodies in the simulation.
  void updateVelocities();
  /// @brief Update the velocities of the bodies in the simulation.
  void updatePositions();
  /// @brief Calculates the average distance of the bodies
  RealVector distanceMean();
  /// @brief Calculates the average distance of the bodies
  /// @param serialBodies A vector containing the position of other bodies.
  RealVector distanceMean(std::vector<double>& serialBodies);
  /// @brief Calculate the standard deviation of the distances of the bodies.
  /// @param serialBodies A vector containing the position of other bodies.
  /// @return A RealVector representing the standard deviation of the distances.
  RealVector distanceStdev(std::vector<double>& serialBodies);
  /// @brief Calculate the standard deviation of the distances of the bodies.
  /// @return A RealVector representing the standard deviation of the distances.
  RealVector distanceStdev(RealVector& mean);
  /// @brief Calculate the mean velocity of the bodies.
  /// @return A RealVector representing the mean velocity of the bodies.
  RealVector velocityMean();
  /// @brief Calculate the mean velocity of the bodies.
  /// @param serialBodies A vector containing the velocity of other bodies.
  RealVector velocityMean(std::vector<double>& serialBodies);

  /// @brief Calculate the standard deviation of the velocities of the bodies.
  /// @return A RealVector representing the standard deviation of the
  /// velocities.
  RealVector velocityStdev(RealVector& mean);

  /// @brief Calculate the standard deviation of the velocities of the bodies.
  /// @param serialBodies A vector containing the serialized velocity.
  /// @return A RealVector representing the standard deviation of the
  /// velocities.
  RealVector velocityStdev(std::vector<double>& serialBodies);

  /// @brief Get the total number of bodies in the simulation.
  /// @return The total number of active bodies.
  size_t remaining() const;

 private:
  /// @brief State of the simulation in wich all processes
  // check for collisions from other broadcasted data.
  void stateColisions();
  /// @brief State of the simulation in wich all processes
  // update the acceleration sum of each body from other broadcasted data.
  void stateAccelerations();
  /// @brief State of the simulation in wich all processes
  // update the velocities and positions of each body
  void statePositions();

 private:
  /// @brief Save the bodies to a tsv file with the current state of the
  // simulation.
  void saveBodiesFile(double currentTime) const;
  /// @brief Generate a report of mean and standard deviation of distances and
  // velocities.
  void reportResults();
};

#endif

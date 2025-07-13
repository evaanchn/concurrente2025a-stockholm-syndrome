// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <string>
#include <vector>

#include "Body.hpp"
#include "common.hpp"
#include "RealVector.hpp"
// #include "Statistics.hpp"
#include "Universe.hpp"

// forward declaration
class Mpi;

class Simulation {
 private:
  double deltaTime = 0.0;
  double maxTime = 0.0;
  int totalBodiesCount = 0;

  /// Container for the bodies in the simulation.
  Universe universe;
  // Statistics statistics;
  /// File containing the universe data.
  std::string universeFile;
  /// Distributed MPI object for parallel processing.
  Mpi* mpi = nullptr;

 public:
  /// @brief Constructor for the Simulation class.
  Simulation() = default;
  DECLARE_RULE4(Simulation, delete);
  ~Simulation();

  int run(int argc, char* argv[]);

 private:
  ExecutionMode analyzeArguments(int argc, char* argv[]);

 private:
  size_t remaining() const;

 private:
  /// @brief State of the simulation in wich all processes
  // check for collisions from other broadcasted data.
  void stateCollisions();
  /// @brief State of the simulation in wich all processes
  // update the acceleration sum of each body from other broadcasted data.
  void stateAccelerations();
  /// @brief State of the simulation in wich all processes
  // update the velocities and positions of each body
  void statePositions();

 private:
  /// @brief Generate a report of mean and standard deviation of distances and
  // velocities.
  /// @param totalActiveBodiesCount The total number of active bodies at the
  // end of simulation.
  void reportResults(const int totalActiveBodiesCount);
};

#endif

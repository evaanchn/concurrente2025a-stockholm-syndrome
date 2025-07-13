// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <string>
#include <vector>

#include "Body.hpp"
#include "common.hpp"
#include "RealVector.hpp"
#include "Universe.hpp"

// forward declaration
class Mpi;

/**
 * @class Simulation
 * @brief Manages the simulation process, including the initialization of
 * bodies, the simulation loop, and the generation of reports.
 */
class Simulation {
  DECLARE_RULE4(Simulation, delete);

 private:
  /// unit of time that increase by state.
  double deltaTime = 0.0;
  /// maximum time allowed for the simulation.
  double maxTime = 0.0;
  /// total number of bodies at the start.
  int totalBodiesCount = 0;
  /// total active bodies in the simulation, currently.
  int totalActiveBodiesCount = 0;

  /// Container for the bodies in the simulation.
  Universe universe;
  /// File containing the universe data.
  std::string universeFile = "";
  /// Distributed MPI object for parallel processing.
  Mpi* mpi = nullptr;

 public:
  /// @brief Constructor for the Simulation class.
  Simulation() = default;
  /// @brief Destructor of the class
  ~Simulation();

  /// @brief Execute the simulation process.
  /// @param argc Number of command line arguments.
  /// @param argv Array of command line arguments.
  /// @return EXIT_SUCCESS if simulated successfully, else EXIT_FAILURE
  int run(int argc, char* argv[]);

 private:
  /// @brief Analyzes arguments and prepares the universe
  /// @see run
  int startSimulation(int argc, char* argv[]);
  /// @brief Attempts to store specified values in attributes, ensuring
  /// that these are correct
  /// @see run for params
  /// @return The execution mode determined after analyzing the arguments
  ExecutionMode analyzeArguments(int argc, char* argv[]);
  /// @brief Carries out main simulation loop
  /// @return The total simulated time
  double simulate();

 private:  // STATES FOR SIMULATE
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
  /// @brief Processes take turns reporting the final states of their universe
  /// @param simulatedTime Total time simulated
  void saveFinalState(double simulatedTime);
  /// @brief Generate a report of mean and standard deviation of distances and
  // velocities.
  /// @param totalActiveBodiesCount The total number of active bodies at the
  // end of simulation.
  void reportResults(const int totalActiveBodiesCount);
};

#endif

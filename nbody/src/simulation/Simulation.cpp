// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "Simulation.hpp"

#include <cstdio>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Mpi.hpp"
#include "Statistics.hpp"
#include "Util.hpp"

// Usage message displayed when program runs in universe file mode
const char* const usage_universe_file =
  "Universe file mode usage: nbody universe_file delta_t max_time\n\n"
  "  universe_file  File with bodies' initial information\n"
  "  delta_t        Duration between each state\n"
  "  max_time       Max time allowed for simulation\n";

// Usage message displayed when program runs in random universe mode
const char* const usage_random_universe =
"Random universe mode usage: "
"nbody bodies_count delta_t max_time min_pos max_pos min_vel max_vel\n\n"
"  bodies_count  File with bodies' initial information\n"
"  delta_t      Duration between each state\n"
"  max_time     Max time allowed for simulation\n"
"  min_mass      Minimum value for body's mass\n"
"  max_mass      Maximum value for body's mass\n"
"  min_rad      Minimum value for body's radius\n"
"  max_rad      Maximum value for body's radius\n"
"  min_pos      Minimum value the initial position can take at x, y, or z\n"
"  max_pos      Maximum value the initial position can take at x, y, or z\n"
"  min_vel      Minimum value the initial velocity can take at x, y, or z\n"
"  max_vel      Maximum value the initial velocity can take at x, y, or z\n";

// Destructor cleans up MPI resources
Simulation::~Simulation() {
  delete this->mpi;
  this->mpi = nullptr;
}

// Main simulation execution function
int Simulation::run(int argc, char* argv[]) {
  // Initialize mpi, analyze args, prepare universe
  if (this->startSimulation(argc, argv) != EXIT_SUCCESS) {
    return EXIT_FAILURE;
  }
  // Simulate states
  double simulatedTime = this->simulate();
  // Save in a file, the final state of the universe
  this->saveFinalState(simulatedTime);
  // Report results
  this->reportResults(this->totalActiveBodiesCount);
  return EXIT_SUCCESS;
}

int Simulation::startSimulation(int argc, char* argv[]) {
  try {
    // Initialize MPI communication
    this->mpi = new Mpi(argc, argv);
    // Load or create initial universe state
    if (this->analyzeArguments(argc, argv) == UNIVERSE_FILE_MODE) {
      // Load universe from file (distributed across processes)
      this->totalBodiesCount = this->universe.loadUniverse(this->universeFile,
        this->mpi->rank(), this->mpi->size());
    } else {
      // Create random universe (distributed across processes)
      this->universe.createUniverse(this->mpi->rank(), this->mpi->size(),
        this->totalBodiesCount);
    }
  } catch (const std::invalid_argument& error) {
    // Handle argument errors
    std::cerr << "error: " << error.what() << std::endl;
    std::cout << usage_universe_file << std::endl;
    std::cout << usage_random_universe << std::endl;
    return EXIT_FAILURE;
  } catch (const std::runtime_error& error) {
    // Handle runtime errors
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

// Analyzes command line arguments to determine execution mode
ExecutionMode Simulation::analyzeArguments(int argc, char* argv[]) {
  // Check minimum argument count
  if (argc < 4)  {
    throw std::invalid_argument("insufficient arguments");
  }

  // Traverse all arguments to check for 'help' specification
  for (int index = 1; index < argc; ++index) {
    const std::string argument = argv[index];
    if (argument.find("help") != std::string::npos) {
      throw std::invalid_argument("help requested");
    }
  }

  // Parse time parameters from arguments
  try {
    this->deltaTime = std::stod(argv[DELTA_T]);
    this->maxTime = std::stod(argv[MAX_TIME]);
    if (this->deltaTime < 0 || this->maxTime < 0) {
      throw std::invalid_argument("negative time values are not permitted");
    }
  } catch(const std::invalid_argument& error) {
    throw std::invalid_argument("invalid time value");
  }

  // First assume program ran in random universe mode
  bool randomUniverseMode = true;
  try {
    this->totalBodiesCount = std::stoul(argv[BODIES_COUNT]);
    // Validate we have enough bodies for all MPI processes
    if (this->totalBodiesCount < this->mpi->size()) {
      throw std::runtime_error("insufficient bodies in universe");
    }
  } catch(const std::invalid_argument& error) {
    // If first argument is not a number, it could be a bodies file
    this->universeFile = argv[UNIVERSE_FILE];
    randomUniverseMode = false;
  }

  if (randomUniverseMode) {
    // Parse remaining random universe parameters
    this->universe.analyzeRandomUniverseModeArguments(argc, argv);
    return RANDOM_UNIVERSE_MODE;
  }
  return UNIVERSE_FILE_MODE;
}

double Simulation::simulate() {
  // Main simulation loop
  double currentTime = 0.0;
  this->totalActiveBodiesCount = this->totalBodiesCount;
  // Simulation loop until max time is reached or only one body remains
  while (currentTime < this->maxTime && this->totalActiveBodiesCount > 1) {
    this->stateCollisions();
    this->stateAccelerations();
    this->statePositions();
    // Synchronize active body count across all processes
    this->mpi->allReduce(this->universe.activeCount(),
      this->totalActiveBodiesCount, MPI_SUM);
    currentTime += this->deltaTime;  // Advance simulation time
  }
  return currentTime;
}

// Handles collision detection and resolution
void Simulation::stateCollisions() {
  // check local collisions
  this->universe.checkCollisions();
  std::vector<double> serializedBodies;
  this->universe.serializeCollisionData(serializedBodies);
  std::vector<double> receivedBodies;
  // broadcast cycle to check colllsions between all processes
  for (int rank  = 0; rank < this->mpi->size(); ++rank) {
    if (rank != mpi->rank()) {
      mpi->broadcast(receivedBodies, rank);
      this->universe.checkCollisions(receivedBodies, this->mpi->rank(), rank);
    } else {
      mpi->broadcast(serializedBodies, rank);
    }
  }
}

void Simulation::stateAccelerations() {
  // check local accelerations
  this->universe.updateAccelerations();
  std::vector<double> serializedBodies;
  this->universe.serializeAccelerationData(serializedBodies);
  std::vector<double> receivedBodies;
  // broadcast cycle to update acceleration between all processes
  for (int rank  = 0; rank < this->mpi->size(); ++rank) {
    if (rank != mpi->rank()) {
      mpi->broadcast(receivedBodies, rank);
      this->universe.updateAccelerations(receivedBodies);
    } else {
      mpi->broadcast(serializedBodies, rank);
    }
  }
}

// Updates body positions based on velocities
void Simulation::statePositions() {
  // Update velocities based on current accelerations
  this->universe.updateVelocities(this->deltaTime);
  // Update positions based on new velocities
  this->universe.updatePositions(this->deltaTime);
}

void Simulation::saveFinalState(double simulatedTime) {
  // Save the final state of the universe
  for (int rank = 0; rank < this->mpi->size(); ++rank) {
    // If it is my turn to report
    if (rank == this->mpi->rank()) {
      // Call universe's saveBodiesFile to write the final states of the bodies
      // in the report file
      this->universe.saveBodiesFile(this->universeFile, simulatedTime
        , this->mpi->rank(), this->totalBodiesCount);
    }
    // Ensure process finishes before moving on
    this->mpi->barrier();
  }
}

// Generates and displays final simulation statistics
void Simulation::reportResults(const int totalActiveBodiesCount) {
  Statistics statistics;
  // Gather distance and velocity data
  const std::vector<RealVector> distances =
      this->universe.getMyDistances(this->mpi);
  const std::vector<RealVector> velocities = this->universe.getMyVelocities();
  // Calculate distributed statistics across all processes
  RealVector distanceMean = statistics.realVectorMeanDistributed(this->mpi,
      distances, totalActiveBodiesCount);
  RealVector distanceStdev = statistics.realVectorStDevDistributed(
    this->mpi, distances, distanceMean, totalActiveBodiesCount);

  RealVector velocityMean = statistics.realVectorMeanDistributed(this->mpi,
      velocities, totalActiveBodiesCount);
  RealVector velocityStdev = statistics.realVectorStDevDistributed(
    this->mpi, velocities, velocityMean, totalActiveBodiesCount);
  // Only first process reports results
  if (this->mpi->rank() != 0) {
    return;
  }
  // Print final statistics
  printf("Remaining bodies: %d\n",
         totalActiveBodiesCount);
  printf("Distance (mean): %s\n", distanceMean.toString().c_str());
  printf("Distance (stdev): %s\n", distanceStdev.toString().c_str());
  printf("Velocity (mean): %s\n", velocityMean.toString().c_str());
  printf("Velocity (stdev): %s\n", velocityStdev.toString().c_str());
}

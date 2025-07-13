// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
#ifndef UNIVERSE_HPP
#define UNIVERSE_HPP

#include <string>
#include <vector>

#include "common.hpp"
#include "Body.hpp"

class Mpi;

/// @class Universe
/// @brief Represents a system containing multiple celestial bodies that
// interact through gravitational forces, supporting simulation updates and MPI
// communication.
class Universe {
  DISABLE_COPY(Universe);  ///< Disable copy constructor and assignment operator

 private:
  // Default limits for random generation of universe bodies
  double minMass = 0.0;
  double maxMass = 0.0;
  double minRadius = 0.0;
  double maxRadius = 0.0;
  double minPosition = 0.0;
  double maxPosition = 0.0;
  double minVelocity = 0.0;
  double maxVelocity = 0.0;
  int activeBodiesCount = 0;  ///< Number of currently active bodies
  std::vector<Body> bodies;   ///< Vector containing all bodies in the universe

 public:
  /// @brief Default constructor.
  Universe() = default;
  ~Universe() = default;

  /// @brief Parse and analyze command line arguments for random universe mode.
  /// @param argc Argument count
  /// @param argv Argument values
  /// @return True if arguments are valid
  bool analyzeRandomUniverseModeArguments(int argc, char* argv[]);

  /// @brief Load universe data from a TSV file.
  /// @param universeFile The input file containing universe data.
  /// @param rank The process rank in MPI.
  /// @param size The total number of processes.
  /// @return Number of bodies loaded by this process.
  size_t loadUniverse(std::string universeFile, size_t rank, size_t size);

  /// @brief Create a randomly generated universe.
  /// @param rank The process rank.
  /// @param size The total number of MPI processes.
  /// @param totalBodiesCount Total number of bodies in the simulation.
  void createUniverse(size_t rank, size_t size, int totalBodiesCount);

  /// @brief Serialize the current state of the simulation for file output.
  /// @param serializedBodies Vector to store the data.
  void serializeCollisionData(std::vector<double>& serializedBodies);

  /// @brief Serialize the acceleration vectors of each body.
  /// @param serializedBodies Vector to store the acceleration data.
  void serializeAccelerationData(std::vector<double>& serializedBodies);

  /// @brief Save the state of the bodies to a file in TSV format.
  /// @param universeFile Output file path.
  /// @param currentTime Current simulation time.
  /// @param rank Rank of the writing process.
  /// @param totalBodyCount Total active bodies in the simulation.
  void saveBodiesFile(std::string universeFile, double currentTime, int rank,
    const int totalBodyCount) const;

  /// @brief Perform collision detection among local bodies.
  void checkCollisions();

  /// @brief Check for collisions between local and remote bodies.
  /// @param serializedBodies Serialized data of external bodies.
  /// @param rank Rank of the current process.
  /// @param otherRank Rank of the other process to compare with.
  void checkCollisions(std::vector<double>& serializedBodies, const int rank,
      const int otherRank);

  /// @brief Update gravitational accelerations of all local bodies.
  void updateAccelerations();

  /// @brief Update accelerations using remote body data.
  /// @param serializedBodies Serialized positions and masses of other bodies.
  void updateAccelerations(std::vector<double>& serializedBodies);

  /// @brief Update velocities of all active bodies.
  /// @param deltaTime Time step for integration.
  void updateVelocities(double deltaTime);

  /// @brief Update positions of all active bodies.
  /// @param deltaTime Time step for integration.
  void updatePositions(double deltaTime);

  /// @brief Apply a generic update function (velocity/position) to all bodies.
  /// @param updateFunc Pointer to the update method.
  /// @param deltaTime Time step used in update.
  void updateBodies(void (Body::*updateFunc)(double), double deltaTime);

  /// @brief Compute all pairwise distances between active local bodies.
  /// @param mpi MPI interface object.
  /// @return Vector of distance vectors.
  std::vector<RealVector> getMyDistances(Mpi* mpi);

  /// @brief Get all local velocities of active bodies.
  /// @return Vector of velocity vectors.
  std::vector<RealVector> getMyVelocities() const;

 private:
  /// @brief Serialize the position vectors of all bodies.
  /// @param serializedPositions Vector to store serialized data.
  void serializePositions(std::vector<double>& serializedPositions);

  /// @brief Compute all pairwise distances among local active bodies.
  /// @param distances Output vector of computed distances.
  void aggregateOwnDistances(std::vector<RealVector>& distances);

  /// @brief Distances between local bodies and remote serialized positions.
  /// @param distances Output vector of computed distances.
  /// @param serializedPositions Serialized positions from another process.
  void aggregateDistances(std::vector<RealVector>& distances,
    const std::vector<double>& serializedPositions);

 public:
  /// @brief Get the number of local bodies in this process.
  /// @return Number of local bodies.
  size_t size() const {
    return this->bodies.size();
  }

  /// @brief Access a body by index (non-const).
  /// @param index Index of the body.
  /// @return Reference to the body.
  Body& operator[](size_t index) {
    return this->bodies[index];
  }

  /// @brief Access a body by index (const).
  /// @param index Index of the body.
  /// @return Constant reference to the body.
  const Body& operator[](size_t index) const {
    return this->bodies[index];
  }

  /// @brief Get the number of active bodies.
  /// @return Active body count.
  int activeCount() const {
    return this->activeBodiesCount;
  }

  /// @brief Get all local bodies.
  /// @return Const reference to the vector of bodies.
  const std::vector<Body>& getBodies() const {
    return this->bodies;
  }

 private:
  /// @brief Compute the start index for static block distribution.
  /// @param rank Process rank.
  /// @param workAmount Total amount of work.
  /// @param workers Number of workers (processes).
  /// @return Start index for the current process.
  size_t calculateStart(int rank, int workAmount, int workers);

  /// @brief Compute the exclusive end index for static block distribution.
  /// @param rank Process rank.
  /// @param workAmount Total amount of work.
  /// @param workers Number of workers (processes).
  /// @return End index (exclusive) for the current process.
  size_t calculateFinish(int rank, int workAmount, int workers);
};

#endif  // UNIVERSE_HPP

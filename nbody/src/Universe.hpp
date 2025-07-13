// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
#ifndef UNIVERSE_HPP
#define UNIVERSE_HPP

#include <string>
#include <vector>
#include "common.hpp"
#include "Body.hpp"

class Mpi;

class Universe {
  DISABLE_COPY(Universe);

 private:
  // default values for the universe bodies
  double minMass = 0.0;
  double maxMass = 0.0;
  double minRadius = 0.0;
  double maxRadius = 0.0;
  double minPosition = 0.0;
  double maxPosition = 0.0;
  double minVelocity = 0.0;
  double maxVelocity = 0.0;
  int activeBodiesCount = 0;
  std::vector<Body> bodies;  // contains the bodies in the universe

 public:
  /// @brief Constructor for Universe.
  Universe() = default;
  ~Universe() = default;

 public:
  bool analyzeRandomUniverseModeArguments(int argc, char* argv[], size_t rank,
    size_t size);
  /// @brief Load the universe from a file.
  size_t loadUniverse(std::string universeFile, size_t rank, size_t size);
  /// @brief create a random universe.
  void createUniverse(size_t rank, size_t size, int totalBodiesCount);
  /// @brief Save the bodies to a tsv file with the current state of the
  // simulation.
  /// @brief Serialize the collision data of the bodies in the simulation.
  void serializeCollisionData(std::vector<double>& serializedBodies);
  /// @brief Serialize the acceleration data of the bodies in the simulation.
  /// @param serializedBodies A vector to store the serialized data.
  void serializeAccelerationData(std::vector<double>& serializedBodies);
  /// @brief Save the bodies to a tsv file
  /// @param universeFile The file to save the bodies to.
  /// @param currentTime The current time of the simulation.
  /// @param rank The rank of the process saving the file.
  /// @param totalBodyCount The total number of active bodies in the simulation.
  void saveBodiesFile(std::string universeFile, double currentTime, int rank,
    const int totalBodyCount) const;

 public:
  /// @brief Check the collisions between bodies in the simulation.
  void checkCollisions();
  /// @brief Check the collisions between bodies in the simulation.
  /// @param serializedBodies A vector containing the mass, radius, position,
  //  and velocities
  /// @param otherRank The rank of the other process to check for collisions.
  /// velocity of other bodie.
  /// @param rank The rank of the current process.
  /// @details This method checks for collisions between bodies in the
  // simulation by comparing their serialized data.
  void checkCollisions(std::vector<double>& serializedBodies, const int rank,
      const int otherRank);
  /// @brief Update the positions of the bodies in the simulation.
  void updateAccelerations();
  /// @brief Update the accelerations of the bodies in the simulation.
  /// @param serializedBodies A vector containing the mass and position
  // of other bodies.
  void updateAccelerations(std::vector<double>& serializedBodies);
  /// @brief Update the velocities of the bodies in the simulation.
  void updateVelocities(double deltaTime);
  /// @brief Update the velocities of the bodies in the simulation.
  void updatePositions(double deltaTime);

 public:
  std::vector<RealVector> getMyDistances(Mpi* mpi);
  std::vector<RealVector> getMyVelocities() const;

 private:
  /// @brief converts positions into a linear sequence of numbers (serialize)
  /// @param serializedPositions positions to be processed
  void serializePositions(std::vector<double>& serializedPositions);

  /// @brief calculates distances
  /// @param distances
  void aggregateOwnDistances(std::vector<RealVector>& distances);

  /// @brief
  /// @param distances
  /// @param serializedPositions
  void aggregateDistances(std::vector<RealVector>& distances,
    const std::vector<double>& serializedPositions);

 public:
  /// @brief
  size_t size() const {
    return this->bodies.size();
  }

  /// @brief
  Body& operator[](size_t index) {
    return this->bodies[index];
  }

  /// @brief
  const Body& operator[](size_t index) const {
    return this->bodies[index];
  }

  /// @brief
  int activeCount() const {
    return this->activeBodiesCount;
  }

  /// @brief Get Bodies
  /// @return The number of active bodies in the simulation.
  const std::vector<Body>& getBodies() const {
    return this->bodies;
  }

 private:
  /// @brief Implements static map by blocks formula, adapted for processes
  /// @param rank Process number
  /// @param workAmount Amount of work to distribute
  /// @param workers Process count
  /// @return Index of start
  size_t calculateStart(int rank, int workAmount, int workers);

  /// @brief Calculates finish index with formula for static map by blocks
  /// @see calculateStart
  /// @return Index of finish (exclusive)
  size_t calculateFinish(int rank, int workAmount, int workers);
};

#endif  // UNIVERSE_HPP

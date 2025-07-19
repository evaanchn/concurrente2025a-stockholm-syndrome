// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "Universe.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <omp.h>  // NOLINT[BUILD-LACK_INCLUDE_SCORE_ORDER]
#include <sstream>
#include <string>
#include <vector>

#include "common.hpp"
#include "Mpi.hpp"
#include "Util.hpp"

// Return true if arguments were set, false if default arguments are needed
bool Universe::analyzeRandomUniverseModeArguments(int argc, char* argv[]) {
  if (argc < 12) {
    return false;  // Not enough arguments for random mode
  }
  try {
    // Parse all range parameters from command line
    this->minMass = std::stod(argv[MIN_MASS]);
    this->maxMass = std::stod(argv[MAX_MASS]);
    this->minRadius = std::stod(argv[MIN_RADIUS]);
    this->maxRadius = std::stod(argv[MAX_RADIUS]);
    this->minPosition = std::stod(argv[MIN_POS]);
    this->maxPosition = std::stod(argv[MAX_POS]);
    this->minVelocity = std::stod(argv[MIN_VEL]);
    this->maxVelocity = std::stod(argv[MAX_VEL]);
  } catch(const std::invalid_argument& error) {
    // If parsing fails, use default values
    std::cerr << "Default minimums and maximums set" << std::endl;
    this->minMass = DEFAULT_MIN_MASS;
    this->maxMass = DEFAULT_MAX_MASS;
    this->minRadius = DEFAULT_MIN_RADIUS;
    this->maxRadius = DEFAULT_MAX_RADIUS;
    this->minPosition = DEFAULT_MIN_POS;
    this->maxPosition = DEFAULT_MAX_POS;
    this->minVelocity = DEFAULT_MIN_VEL;
    this->maxVelocity = DEFAULT_MAX_VEL;
  }
  return true;
}

// Loads universe state from file, distributing bodies across MPI processes
size_t Universe::loadUniverse(std::string universeFile, size_t rank,
    size_t size) {
  size_t totalBodyCount = 0;
  std::ifstream file(universeFile);
  if (!file) {
    std::cerr << "Cannot open file";
    throw std::invalid_argument("cannot open universe file");
  }
  try {
    std::string line;
    // First line contains total body count
    std::getline(file, line);
    totalBodyCount = std::stoul(line);
    // Validate we have enough bodies for all processes
    if (totalBodyCount < size) {
      throw std::runtime_error("insufficient bodies in universe");
    }

    size_t processBodiesCount = this->setUniverseFileLimits(file, rank, size,
        totalBodyCount);
    // Read only the bodies assigned to this process
    for (size_t index = 0; index < processBodiesCount; ++index) {
      this->loadBody(file, line);
    }
  } catch (const std::invalid_argument& error) {
    file.close();
    throw std::invalid_argument("invalid body arguments in universe file");
  }
  file.close();
  this->activeBodiesCount = this->bodies.size();
  return totalBodyCount;
}

size_t Universe::setUniverseFileLimits(std::ifstream& file, size_t rank,
    size_t size, size_t totalBodyCount) {
  std::string line;
  // Calculate which bodies this process should handle
  size_t processBodiesStart = Util::calculateStart(rank,
    totalBodyCount, size);
  size_t processBodiesEnd = Util::calculateFinish(rank,
    totalBodyCount, size);
  // Skip bodies not assigned to this process
  for (size_t index = 0; index < processBodiesStart; ++index) {
    std::getline(file, line);
  }
  return processBodiesEnd - processBodiesStart;
}

void Universe::loadBody(std::ifstream& file, std::string& line) {
  if (!std::getline(file, line)) {
    return;  // End of file reached
  }

  // Parse tab-separated body data
  std::istringstream sstream(line);
  std::string cell;
  std::vector<std::string> row;
  while (std::getline(sstream, cell, '\t')) {
    row.push_back(cell);
  }
  // Util::split(row, "\t", true);
  // Create and store new body
  this->bodies.push_back(Body(
    std::stod(row[0]),  // mass
    std::stod(row[1]),  // radius
    // position (x, y, z)
    RealVector(std::stod(row[2]), std::stod(row[3]), std::stod(row[4])),
    // velocity(x, y, z)
    RealVector(std::stod(row[5]), std::stod(row[6]), std::stod(row[7]))));
}

// Creates a random universe with bodies distributed across MPI processes
void Universe::createUniverse(size_t rank, size_t size, int totalBodiesCount) {
  // Calculate range of bodies this process should create
  int start = Util::calculateStart(rank, totalBodiesCount, size);
  int finish =  Util::calculateFinish(rank, totalBodiesCount, size);
  int myBodiesCount = finish - start;
  // Generate random bodies within specified parameter ranges
  for (int index = 0; index < myBodiesCount; ++index) {
    // Generate random values for new body, between specified ranges
    double mass = Util::random(this->minMass, this->maxMass);
    double radius = Util::random(this->minRadius, this->maxRadius);
    // Randdom position
    double positionX = Util::random(this->minPosition,
        this->maxPosition);
    double positionY = Util::random(this->minPosition,
        this->maxPosition);
    double positionZ = Util::random(this->minPosition,
        this->maxPosition);
    // Random velocity
    double velocityX = Util::random(this->minVelocity,
        this->maxVelocity);
    double velocityY = Util::random(this->minVelocity,
        this->maxVelocity);
    double velocityZ = Util::random(this->minVelocity,
        this->maxVelocity);
    // Create and store new random body
    this->bodies.push_back(Body(
      mass,
      radius,
      RealVector(positionX, positionY, positionZ),
      RealVector(velocityX, velocityY, velocityZ)));
  }
  // Set current active bodies count as amount of bodies created
  this->activeBodiesCount = this->bodies.size();
}


// Serializes body data for collision detection
void Universe::serializeCollisionData(std::vector<double>& serializedBodies) {
  for (const Body& body : this->bodies) {
    if (body.isActive()) {
      body.serializeCheckCollision(serializedBodies);
    }
  }
}

// Serializes body data for acceleration calculations
void Universe::serializeAccelerationData
  (std::vector<double>& serializedBodies) {
  for (const Body& body : this->bodies) {
    if (body.isActive()) {
      body.serializeAccelerationData(serializedBodies);
    }
  }
}

// Saves current universe state to file (parallel version)
void Universe::saveBodiesFile(std::string universeFile,
    double currentTime, const int rank, const int totalBodyCount) const {
  // remove the file extension from the universe file
  std::string fileName = universeFile.substr(0, universeFile.find_last_of('.'));
  fileName += "-" +std::to_string(static_cast<int>(currentTime)) + ".tsv";
  // Append if rank != 0
  std::ofstream file = std::ofstream();
  if (rank == 0) {
    // Restart and write the total body totalActiveCount only once
    file.open(fileName);
    file << totalBodyCount << std::endl;
  } else {
    file.open(fileName, std::ios::app);
  }
  if (!file) {
    std::cerr << "Cannot open file for writing: " << fileName << std::endl;
    throw std::runtime_error("cannot save bodies file");
  }
  // Write all bodies managed by this process
  for (const Body& body : this->bodies) {
    file << body << std::endl;
  }
  file.close();
}

// Checks for collisions between local bodies
void Universe::checkCollisions() {
  for (size_t index = 0; index < this->bodies.size(); ++index) {
    if (!this->bodies[index].isActive()) {
      continue;  // Skip inactive bodies
    }
    // Check against all other bodies
    for (size_t other_index = 0; other_index < this->bodies.size();
        ++other_index) {
      if (index == other_index || !this->bodies[other_index].isActive()) {
        continue;  // Skip self-comparison
      }
      if (this->bodies[index].checkCollision(this->bodies[other_index])) {
        // #pragma omp critical
        --this->activeBodiesCount;
        // Let the more massive body absorb the smaller one
        if (!this->bodies[index].absorb(this->bodies[other_index])) {
          this->bodies[other_index].absorb(this->bodies[index]);
        }
      }
    }
  }
}

void Universe::checkCollisions(std::vector<double>& serializedBodies,
    const int rank, const int otherRank) {
  // Parallel for iterating through every local bodies
  // omitting default(none) given no private data
  // No race conditions given bodies from other processes are not modified.
  #pragma omp parallel for num_threads(omp_get_max_threads()) schedule(dynamic)
  for (size_t index = 0; index < this->bodies.size(); ++index) {
    if (!this->bodies[index].isActive()) {
      continue;  // Skip if current body is not active
    }
    // Check collision for current body with every single serialized body sent
    for (size_t offset = 0; offset < serializedBodies.size();
        offset += BODY_COLLISION_DATA_SIZE) {
      // Build auxiliary vectors for position and velocity to check collision
      RealVector otherPosition = RealVector(serializedBodies[offset +
        COLLISION_POSITION_X],  serializedBodies[offset + COLLISION_POSITION_Y],
        serializedBodies[offset +  COLLISION_POSITION_Z]);
      RealVector otherVelocity = RealVector(serializedBodies[offset +
        COLLISION_VELOCITY_X],  serializedBodies[offset + COLLISION_VELOCITY_Y],
        serializedBodies[offset + COLLISION_VELOCITY_Z]);

      // If the current body collides
      if (this->bodies[index].checkCollision(
          serializedBodies[offset + COLLISION_RADIUS], otherPosition)) {
        this->collideBodies(this->bodies[index], serializedBodies,
            otherVelocity, offset, rank, otherRank);
        break;
      }
    }
  }
}

void Universe::collideBodies(Body& body, std::vector<double>& serializedBodies
    , const RealVector& otherVelocity, const size_t offset, const int rank,
    const int otherRank) {
  // Get other body's mass
  double otherMass = serializedBodies[offset + COLLISION_MASS];
  // If both have equal masses, the one from lower rank absorbs the one
  // managed by a process with higher rank. Skip if my rank is lower
  if (body.equalMasses(otherMass) && rank < otherRank) {
    body.deactivate();
    #pragma omp atomic
    --this->activeBodiesCount;
    return;  // Skip the absorb attempt
  }

  // If this body could not absorb the other process's one, deactivate
  if (!body.absorb(otherMass,
      serializedBodies[offset + COLLISION_RADIUS], otherVelocity)) {
    body.deactivate();
    #pragma omp atomic
    --this->activeBodiesCount;
  }
}

void Universe::updateAccelerations() {
  std::vector<Body>& tempBodies = this->bodies;
  #pragma omp parallel num_threads(omp_get_max_threads()) \
    default(none) shared(tempBodies)
  {
    // Must reset accelerations separately first
    this->resetAccelerations(tempBodies);
    #pragma omp barrier
    // After ensuring all accelerations have been reset, update
    this->updateLocalAccelerations(tempBodies);
  }
}

void Universe::resetAccelerations(std::vector<Body>& tempBodies) {
  // First reset every body's acceleration
  #pragma omp for schedule(dynamic)
  for (Body& body : tempBodies) {
    // Only do so if body is active
    if (body.isActive()) {
      body.resetAcceleration();
    }
  }
}

void Universe::updateLocalAccelerations(std::vector<Body>& tempBodies) {
  #pragma omp for schedule(dynamic)
  for (size_t i = 0; i < tempBodies.size(); ++i) {
    Body& body = tempBodies[i];
    if (!body.isActive()) {
      continue;  // Skip inactive bodies
    }

    for (size_t j = 0; j < tempBodies.size(); ++j) {
      if (i == j || !tempBodies[j].isActive()) {
        continue;  // Skip self-comparison and inactive bodies
      }
      body.updateAcceleration(tempBodies[j]);
    }
  }
}

void Universe::updateAccelerations(std::vector<double>& serializedBodies) {
  // Local alias so omp's shared can use inside parallel for
  std::vector<Body>& localBodies = this->bodies;
  // Dynamic map distribution between threads given some bodies don't need to be
  // evaluated if inactive
  #pragma omp parallel for num_threads(omp_get_max_threads()) \
    default(none) shared(localBodies, serializedBodies) schedule(dynamic)
  for (size_t i = 0; i < localBodies.size(); ++i) {
    Body& body = localBodies[i];  // Represents current body
    if (!body.isActive()) {
      continue;  // Skip inactive bodies
    }

    // Evaluate with data from every body from other process
    for (size_t offset = 0; offset < serializedBodies.size();
        offset += BODY_ACCELERATION_DATA_SIZE) {
      // Create a real vector to represent other body's acceleration
      RealVector otherPosition =
        RealVector(serializedBodies[offset + ACCELERATION_POSITION_X],
          serializedBodies[offset + ACCELERATION_POSITION_Y],
          serializedBodies[offset + ACCELERATION_POSITION_Z]);

      body.updateAcceleration(serializedBodies[offset + ACCELERATION_MASS],
        otherPosition);
    }
  }
}


void Universe::updateVelocitiesAndPositions(double deltaTime) {
  // Local alias so omp's shared can use inside parallel for
  std::vector<Body>& tempBodies = this->bodies;
  #pragma omp parallel for num_threads(omp_get_max_threads()) \
    default(none) shared(tempBodies, deltaTime) schedule(dynamic)
  for (size_t index = 0; index < tempBodies.size(); ++index) {
    Body& currentBody = tempBodies[index];
    if (!currentBody.isActive()) {
      continue;  // Skip inactive bodies
    }
    currentBody.updateVelocity(deltaTime);  // Update velocity first
    currentBody.updatePosition(deltaTime);  // Update position accordingly
  }
}

std::vector<RealVector> Universe::getMyDistances(Mpi* mpi) {
  std::vector<RealVector> distances;  // Create vector of distances
  this->aggregateOwnDistances(distances);  // First add own distances
  // Prepare serialized distances
  std::vector<double> serializedPositions;
  this->serializePositions(serializedPositions);
  std::vector<double> receivedPositions;
  // Take turns between processes
  for (int currentRank = 0; currentRank < mpi->size(); ++currentRank) {
    // If it's my turn to broadcast, send serialized positions to others
    if (currentRank == mpi->rank()) {
      mpi->broadcast(serializedPositions, currentRank);
    } else {
      // Receive broadcasted positions from current rank
      mpi->broadcast(receivedPositions, currentRank);
      // If my rank is greater than the current sending rank, consider the
      // distances between each others' bodies, with the purpose of not
      // not repeating distances
      if (mpi->rank() > currentRank) {
        this->aggregateDistances(distances, receivedPositions);
      }
    }
  }
  return distances;
}

void Universe::serializePositions(std::vector<double>& serializedPositions) {
  for (const Body& body : this->bodies) {
    // Only serialize active bodies' positions
    if (body.isActive()) {
      // Serialize position data
      body.serializePositionData(serializedPositions);
    }
  }
}

void Universe::aggregateOwnDistances(std::vector<RealVector>& distances) {
  // Iterate through vector of bodies
  for (size_t startBodyIdx = 0; startBodyIdx < bodies.size() - 1;
      ++startBodyIdx) {
    // Skip iteration if starting body is not active
    if (!this->bodies[startBodyIdx].isActive()) {
      continue;
    }
    // Iterate through bodies after the starting body
    for (size_t currentBodyIdx = startBodyIdx + 1; currentBodyIdx <
        this->bodies.size(); ++currentBodyIdx) {
      if (startBodyIdx == currentBodyIdx) {
        continue;
      }
      // Only calculate distances with active bodies
      if (this->bodies[currentBodyIdx].isActive()) {
        // Distance calculation
        distances.push_back(this->bodies[startBodyIdx].getPosition() -
            this->bodies[currentBodyIdx].getPosition());
      }
    }
  }
}

void Universe::aggregateDistances(std::vector<RealVector>& distances,
    const std::vector<double>& serializedPositions) {
  // For every body in this process, evaluate distances from other bodies
  for (size_t myBodyIdx = 0; myBodyIdx < this->bodies.size(); ++myBodyIdx) {
    // Only add distance to sum if currentBody is active
    if (this->bodies[myBodyIdx].isActive()) {
      // Iterate through every serialized position
      for (size_t offset = 0; offset < serializedPositions.size();
          offset += BODY_DISTANCE_DATA_SIZE) {
        // Build auxiliary position
        std::vector<double> auxPosition = {serializedPositions[offset],
          serializedPositions[offset + 1], serializedPositions[offset + 2]};
        // Calculate distance and add to distances vector
        distances.push_back(RealVector(auxPosition) -
            this->bodies[myBodyIdx].getPosition());
      }
    }
  }
}

std::vector<RealVector> Universe::getMyVelocities() const {
  std::vector<RealVector> velocities;
  // Allocate space needed for the vector to return
  velocities.reserve(this->activeBodiesCount);
  // Iterate through every body stored
  for (const Body& body : this->bodies) {
    // Add body's velocity vector to collection
    velocities.push_back(body.getVelocity());
  }
  return velocities;
}

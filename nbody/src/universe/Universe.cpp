// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "Universe.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <omp.h>  // NOLINT[BUILD-LACK_INCLUDE_SCORE_ORDER]
#include <sstream>
#include <string>
#include <vector>

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
    // Calculate which bodies this process should handle
    size_t process_bodies_start = this->calculateStart(rank,
        totalBodyCount, size);
    size_t process_bodies_end = this->calculateFinish(rank,
        totalBodyCount, size);
    // Skip bodies not assigned to this process
    for (size_t index = 0; index < process_bodies_start; ++index) {
      std::getline(file, line);
    }
    // Read only the bodies assigned to this process
    for (size_t index = process_bodies_start; index < process_bodies_end;
        ++index) {
      if (!std::getline(file, line)) {
        break;  // End of file reached
      }
      // Parse tab-separated body data
      std::istringstream sstream(line);
      std::string cell;
      std::vector<std::string> row;
      while (std::getline(sstream, cell, '\t')) {
        row.push_back(cell);
      }
      // Extract position and velocity vectors
      std::vector<double> auxPos = {std::stod(row[2]), std::stod(row[3]),
        std::stod(row[4])};
      std::vector<double> auxVelocity = {std::stod(row[5]), std::stod(row[6]),
        std::stod(row[7])};
      // Create and store new body
      this->bodies.push_back(Body(
        std::stod(row[0]),  // mass
        std::stod(row[1]),  // radius
        RealVector(auxPos),  // position
        RealVector(auxVelocity)));  // velocity
    }
  } catch (const std::invalid_argument& error) {
    file.close();
    throw std::invalid_argument("invalid body arguments in universe file");
  }
  file.close();
  this->activeBodiesCount = this->bodies.size();
  return totalBodyCount;
}

// Creates a random universe with bodies distributed across MPI processes
void Universe::createUniverse(size_t rank, size_t size, int totalBodiesCount) {
  // Calculate range of bodies this process should create
  int start = this->calculateStart(rank, totalBodiesCount, size);
  int finish =  this->calculateFinish(rank, totalBodiesCount, size);
  int myBodiesCount = finish - start;
  // Generate random bodies within specified parameter ranges
  for (int index = 0; index < myBodiesCount; ++index) {
    double mass = Util::random(this->minMass, this->maxMass);
    double radius = Util::random(this->minRadius, this->maxRadius);
    std::vector<double> positionVector;
    std::vector<double> velocityVector;
    // Generate random position and velocity components
    for (int i = 0; i < 3; ++i) {
      positionVector.push_back(Util::random(this->minPosition,
          this->maxPosition));
      velocityVector.push_back(Util::random(this->minVelocity,
          this->maxVelocity));
    }

    // Create and store new random body
    this->bodies.push_back(Body(
      mass,
      radius,
      RealVector(positionVector),
      RealVector(velocityVector)));
  }
  this->activeBodiesCount = this->bodies.size();
}

// Calculates start index for work distribution among processes
size_t Universe::calculateStart(int rank, int workAmount, int workers) {
  // Add the residue if the process number exceeds it
  size_t added = rank < workAmount % workers ? rank : workAmount % workers;
  return rank * (workAmount / workers) + added;
}

// Calculates end index for work distribution
size_t Universe::calculateFinish(int rank, int workAmount, int workers) {
  return calculateStart(rank + 1, workAmount, workers);
}

// Serializes body data for collision detection
void Universe::serializeCollisionData(std::vector<double>& serializedBodies) {
  for (const Body& body : this->bodies) {
    if (body.isActive()) {
      // Append each active body's collision data to the vector
      std::vector<double> serializedBody = body.serializeCheckCollision();
      serializedBodies.insert(serializedBodies.end(), serializedBody.begin(),
        serializedBody.end());
    }
  }
}

// Serializes body data for acceleration calculations
void Universe::serializeAccelerationData
  (std::vector<double>& serializedBodies) {
  for (const Body& body : this->bodies) {
    if (body.isActive()) {
      // Append each active body's acceleration data to the vector
      std::vector<double> serializedBody = body.serializeAccelerationData();
      serializedBodies.insert(serializedBodies.end(), serializedBody.begin(),
        serializedBody.end());
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
  #pragma omp parallel for num_threads(omp_get_max_threads())
  for (size_t index = 0; index < this->bodies.size(); ++index) {
    if (!this->bodies[index].isActive()) {
      continue;  // Skip if current body is not active
    }
    // Check collision for current body with every single serialized body sent
    for (size_t offset = 0; offset < serializedBodies.size();
        offset += BODY_COLLISION_DATA_SIZE) {
      // Build auxiliary vectors for position and velocity to check collision
      std::vector<double> auxPosition = {serializedBodies[offset +
        COLLISION_POSITION_X],  serializedBodies[offset + COLLISION_POSITION_Y],
        serializedBodies[offset +  COLLISION_POSITION_Z]};
      std::vector<double> auxVelocity = {serializedBodies[offset +
        COLLISION_VELOCITY_X],  serializedBodies[offset + COLLISION_VELOCITY_Y],
        serializedBodies[offset + COLLISION_VELOCITY_Z]};

      // If the current body collides
      if (this->bodies[index].checkCollision(
          serializedBodies[offset + COLLISION_RADIUS],
            RealVector(auxPosition))) {
        // Get other body's mass
        double otherMass = serializedBodies[offset + COLLISION_MASS];
        // If both have equal masses, the one from lower rank absorbs the one
        // managed by a process with higher rank. Skip if my rank is lower
        if (this->bodies[index].equalMasses(otherMass) && rank < otherRank) {
          this->bodies[index].deactivate();
          #pragma omp atomic
          --this->activeBodiesCount;
          break;  // The current body should no longer be evaluated
        }
        // If this body could not absorb the other process's one, deactivate
        if (!this->bodies[index].absorb(otherMass,
            serializedBodies[offset + COLLISION_RADIUS],
            RealVector(auxVelocity))) {
          this->bodies[index].deactivate();
          #pragma omp atomic
          --this->activeBodiesCount;
          break;   // The current body should no longer be evaluated
        }
      }
    }
  }
}

void Universe::updateAccelerations() {
  for (size_t i = 0; i < this->bodies.size(); ++i) {
    Body& body = this->bodies[i];
    if (!body.isActive()) {
      continue;  // Skip inactive bodies
    }
    // Reset acceleration to zero before recalculating
    body.resetAcceleration();
    for (size_t j = 0; j < this->bodies.size(); ++j) {
      if (i == j || !this->bodies[j].isActive()) {
        continue;  // Skip self-comparison and inactive bodies
      }
      body.updateAcceleration(this->bodies[j]);
    }
  }
}

void Universe::updateAccelerations(std::vector<double>& serializedBodies) {
  // Local alias so omp's shared can use inside parallel for
  std::vector<Body>& localBodies = this->bodies;

  #pragma omp parallel for num_threads(omp_get_max_threads()) \
    default(none) shared(localBodies, serializedBodies)
  for (size_t i = 0; i < localBodies.size(); ++i) {
    Body& body = localBodies[i];
    if (!body.isActive()) {
      continue;  // Skip inactive bodies
    }
    for (size_t offset = 0; offset < serializedBodies.size();
        offset += BODY_ACCELERATION_DATA_SIZE) {
      std::vector<double> auxPosition =
        {serializedBodies[offset + ACCELERATION_POSITION_X],
          serializedBodies[offset + ACCELERATION_POSITION_Y],
          serializedBodies[offset + ACCELERATION_POSITION_Z]};

      body.updateAcceleration(serializedBodies[offset + ACCELERATION_MASS],
        RealVector(auxPosition));
    }
  }
}

// MODULARIZED USING CHAT GPT
void Universe::updateBodies(void (Body::*updateFunc)(double),
    double deltaTime) {
  // Local alias so omp's shared can use inside parallel for
  std::vector<Body>& tempBodies = this->bodies;

  #pragma omp parallel for num_threads(omp_get_max_threads()) \
    default(none) shared(tempBodies, deltaTime, updateFunc)
  for (size_t index = 0; index < tempBodies.size(); ++index) {
    (tempBodies[index].*updateFunc)(deltaTime);  // Update using specified func
  }
}

void Universe::updateVelocities(double deltaTime) {
  updateBodies(&Body::updateVelocity, deltaTime);
}

void Universe::updatePositions(double deltaTime) {
  updateBodies(&Body::updatePosition, deltaTime);
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
      std::vector<double> serializedPosition = body.serializePositionData();
      // Insert into vector of serialized positions
      serializedPositions.insert(serializedPositions.end(),
        serializedPosition.begin(), serializedPosition.end());
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
  std::vector<RealVector> velocities = {};
  for (const Body& body : this->bodies) {
    velocities.push_back(body.getVelocity());
  }
  return velocities;
}

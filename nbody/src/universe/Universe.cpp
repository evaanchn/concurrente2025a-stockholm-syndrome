// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "Universe.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Mpi.hpp"
#include "Util.hpp"

// Return true if arguments were set, false if default arguments are needed
bool Universe::analyzeRandomUniverseModeArguments(int argc, char* argv[]) {
  if (argc < 12) {
    return false;
  }
  try {
    this->minMass = std::stod(argv[MIN_MASS]);
    this->maxMass = std::stod(argv[MAX_MASS]);
    this->minRadius = std::stod(argv[MIN_RADIUS]);
    this->maxRadius = std::stod(argv[MAX_RADIUS]);
    this->minPosition = std::stod(argv[MIN_POS]);
    this->maxPosition = std::stod(argv[MAX_POS]);
    this->minVelocity = std::stod(argv[MIN_VEL]);
    this->maxVelocity = std::stod(argv[MAX_VEL]);
  } catch(const std::invalid_argument& error) {
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
    std::getline(file, line);
    totalBodyCount = std::stoul(line);
    if (totalBodyCount < size) {
      throw std::runtime_error("insufficient bodies in universe");
    }
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
      std::istringstream sstream(line);
      std::string cell;
      std::vector<std::string> row;
      while (std::getline(sstream, cell, '\t')) {
        row.push_back(cell);
      }
      std::vector<double> auxPos = {std::stod(row[2]), std::stod(row[3]),
        std::stod(row[4])};
      std::vector<double> auxVelocity = {std::stod(row[5]), std::stod(row[6]),
        std::stod(row[7])};
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

void Universe::createUniverse(size_t rank, size_t size, int totalBodiesCount) {
  int start = this->calculateStart(rank, totalBodiesCount, size);
  int finish =  this->calculateFinish(rank, totalBodiesCount, size);
  int myBodiesCount = finish - start;
  for (int index = 0; index < myBodiesCount; ++index) {
    double mass = Util::random(this->minMass, this->maxMass);
    double radius = Util::random(this->minRadius, this->maxRadius);
    std::vector<double> positionVector;
    std::vector<double> velocityVector;
    for (int i = 0; i < 3; ++i) {
      positionVector.push_back(Util::random(this->minPosition,
          this->maxPosition));
      velocityVector.push_back(Util::random(this->minVelocity,
          this->maxVelocity));
    }

    this->bodies.push_back(Body(
      mass,
      radius,
      RealVector(positionVector),
      RealVector(velocityVector)));
  }
  this->activeBodiesCount = this->bodies.size();
}

size_t Universe::calculateStart(int rank, int workAmount, int workers) {
  // Add the residue if the process number exceeds it
  size_t added = rank < workAmount % workers ? rank : workAmount % workers;
  return rank * (workAmount / workers) + added;
}

size_t Universe::calculateFinish(int rank, int workAmount, int workers) {
  return calculateStart(rank + 1, workAmount, workers);
}

void Universe::serializeCollisionData(std::vector<double>& serializedBodies) {
  for (const Body& body : this->bodies) {
    if (body.isActive()) {
      std::vector<double> serializedBody = body.serializeCheckCollision();
      serializedBodies.insert(serializedBodies.end(), serializedBody.begin(),
        serializedBody.end());
    }
  }
}

void Universe::serializeAccelerationData
  (std::vector<double>& serializedBodies) {
  for (const Body& body : this->bodies) {
    if (body.isActive()) {
      std::vector<double> serializedBody = body.serializeAccelerationData();
      serializedBodies.insert(serializedBodies.end(), serializedBody.begin(),
        serializedBody.end());
    }
  }
}

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
  for (const Body& body : this->bodies) {
    file << body << std::endl;
  }
  file.close();
}

void Universe::checkCollisions() {
  // #pragma omp parallel for num_threads(this->threadCount) \
  //   default(none) shared(this->bodies, this->activeBodiesCount)
  for (size_t index = 0; index < this->bodies.size(); ++index) {
    if (!this->bodies[index].isActive()) {
      continue;  // Skip inactive bodies
    }
    for (size_t other_index = 0; other_index < this->bodies.size();
        ++other_index) {
      if (index == other_index || !this->bodies[other_index].isActive()) {
        continue;  // Skip self-comparison
      }
      if (this->bodies[index].checkCollision(this->bodies[other_index])) {
        // #pragma omp critical
        --this->activeBodiesCount;
        if (!this->bodies[index].absorb(this->bodies[other_index])) {
          this->bodies[other_index].absorb(this->bodies[index]);
        }
      }
    }
  }
}

void Universe::checkCollisions(std::vector<double>& serializedBodies,
    const int rank, const int otherRank) {
  for (size_t index = 0; index < this->bodies.size(); ++index) {
    if (!this->bodies[index].isActive()) {
      continue;
    }
    for (size_t offset = 0; offset < serializedBodies.size();
    offset += BODY_COLLISION_DATA_SIZE) {
      std::vector<double> auxPosition = {serializedBodies[offset +
        COLLISION_POSITION_X],  serializedBodies[offset + COLLISION_POSITION_Y],
        serializedBodies[offset +  COLLISION_POSITION_Z]};
      std::vector<double> auxVelocity = {serializedBodies[offset +
        COLLISION_VELOCITY_X],  serializedBodies[offset + COLLISION_VELOCITY_Y],
        serializedBodies[offset + COLLISION_VELOCITY_Z]};

      if (this->bodies[index].checkCollision(
          serializedBodies[offset + COLLISION_RADIUS],
            RealVector(auxPosition))) {
        double otherMass = serializedBodies[offset + COLLISION_MASS];
        if (this->bodies[index].equalMasses(otherMass) &&
            rank > otherRank) {
          this->bodies[index].deactivate();
          --this->activeBodiesCount;
          continue;
        }
        if (!this->bodies[index].absorb(otherMass,
            serializedBodies[offset + COLLISION_RADIUS],
            RealVector(auxVelocity))) {
          this->bodies[index].deactivate();
          std::cout << "Deactivating body: " << this->bodies[index] <<
            std::endl;
          --this->activeBodiesCount;
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
  for (size_t i = 0; i < this->bodies.size(); ++i) {
    Body& body = this->bodies[i];
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

void Universe::updateVelocities(double deltaTime) {
  for (Body& body : this->bodies) {
    body.updateVelocity(deltaTime);
  }
}

void Universe::updatePositions(double deltaTime) {
  for (Body& body : this->bodies) {
    body.updatePosition(deltaTime);
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
  // TODO(evan) add OMP for static map reduce
  // For every body in this process, evaluate distances from other bodies
  for (size_t myBodyIdx = 0; myBodyIdx < this->bodies.size(); ++myBodyIdx) {
    // Only add distance to sum if currentBody is active
    if (this->bodies[myBodyIdx].isActive()) {
      // Iterate through every serialized position
      // TODO(evan) check if offset does start at 1
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

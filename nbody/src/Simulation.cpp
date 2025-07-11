// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "Simulation.hpp"

#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <iostream>

// #include "Mpi.hpp"
#include "Random.hpp"

const char* const usage_universe_file =
  "Usage: nbody universe_file delta_t max_time\n\n"
  "  universe_file  File with bodies' initial information\n"
  "  delta_t        Duration between each state\n"
  "  max_time       Max time allowed for simulation\n";

const char* const usage_random_universe =
"Usage: nbody bodies_count delta_t max_time min_pos max_pos min_vel max_vel\n\n"
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

ExecutionMode Simulation::analyzeArguments(int argc, char* argv[]) {
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

  try {
    this->deltaTime = std::stod(argv[DELTA_T]);
    this->maxTime = std::stod(argv[MAX_TIME]);
  } catch(const std::invalid_argument& error) {
    throw std::invalid_argument("invalid time value");
  }

  // First assume program ran in random universe mode
  bool randomUniverseMode = true;
  try {
    this->totalBodyCount = std::stoul(argv[BODIES_COUNT]);
  } catch(const std::invalid_argument& error) {
    // If first argument is not a number, it could be a bodies file
    this->universeFile = argv[UNIVERSE_FILE];
    randomUniverseMode = false;
  }

  if (randomUniverseMode) {
    if (!analyzeRandomUniverseModeArguments(argc, argv)) {
      // TODO(you): ensure only process 0 prints
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
    return RANDOM_UNIVERSE_MODE;
  }
  return UNIVERSE_FILE_MODE;
}

// Return true if arguments were set, false if default arguments are needed
bool Simulation::analyzeRandomUniverseModeArguments(int argc, char* argv[]) {
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
    return false;
  }
  return true;
}

void Simulation::loadUniverse() {
  // TODO(any) Adjust for distribution, start at block assigned body and load
  // only amount of bodies it will handle
  std::ifstream file(this->universeFile);
  if (!file) {
    std::cerr << "Cannot open file";
    throw std::invalid_argument("cannot open universe file");
  }
  try {
    std::string line;
    std::getline(file, line);
      this->totalBodyCount = std::stoul(line);
    // std::getline(file, line);
    while (std::getline(file, line)) {
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
}

void Simulation::createUniverse() {
  // TODO(any) Adjust for distribution, remove totalBodyCount and create only
  // amount of bodies it will handle
  for (size_t index = 0; index < this->totalBodyCount; ++index) {
    Random<double> random;
    double mass = random.between(this->minMass, this->maxMass);
    double radius = random.between(this->minRadius, this->maxRadius);

    std::vector<double> positionVector;
    std::vector<double> velocityVector;
    for (int i = 0; i < 3; ++i) {
      positionVector.push_back(random.between(this->minPosition,
          this->maxPosition));
      velocityVector.push_back(random.between(this->minVelocity,
          this->maxVelocity));
    }

    this->bodies.push_back(Body(
      mass,
      radius,
      RealVector(positionVector),
      RealVector(velocityVector)));
  }
}

void Simulation::run(int argc, char* argv[]) {
  if (this->analyzeArguments(argc, argv) == UNIVERSE_FILE_MODE) {
    this->loadUniverse();
  } else {
    this->createUniverse();
  }
  this->activeBodiesCount = this->bodies.size();
  printf("Simulation started with %zu bodies.\n", this->bodies.size());
  double currentTime = 0.0;
  while (currentTime < this->maxTime && this->activeBodiesCount > 1) {
    // TODO(any) Adjust for distribution, only process 0 prints
    this->checkCollisions();
    this->updateVelocities();
    this->updatePositions();
    currentTime += this->deltaTime;
  }
  this->saveBodiesFile(currentTime);
  this->reportResults();
}

void Simulation::checkCollisions() {
  for (Body& body : this->bodies) {
    for (Body& otherBody : this->bodies) {
      if (body == otherBody || !body.isActive() || !otherBody.isActive()) {
        continue;  // Skip self-comparison
      }
      if (body.checkCollision(otherBody)) {
        --this->activeBodiesCount;
        if (!body.absorb(otherBody)) {
          otherBody.absorb(body);
        }
      }
    }
  }
}

void Simulation::checkCollisions(std::vector<double>& serialBodies) {
  for (Body& body : this->bodies) {
    for (size_t offset = 1; offset < serialBodies.size();
        offset += BODY_COLLISION_DATA_SIZE) {
      std::vector<double> auxPosition = {serialBodies[offset +
        COLLISION_POSITION_X],  serialBodies[COLLISION_POSITION_Y],

        serialBodies[offset +  COLLISION_POSITION_Z]};
      std::vector<double> auxVelocity = {serialBodies[offset +
        COLLISION_VELOCITY_X],  serialBodies[COLLISION_VELOCITY_Y],

        serialBodies[offset + COLLISION_VELOCITY_Z]};

      if (!body.isActive()) {
        continue;
      }

      if (body.checkCollision(serialBodies[offset + COLLISION_RADIUS],
        RealVector(auxPosition))) {
        if (body.absorb(serialBodies[offset + COLLISION_MASS],
          serialBodies[COLLISION_RADIUS], RealVector(auxPosition))) {
        } else {
          --this->activeBodiesCount;
        }
      }
    }
  }
}

void Simulation::updateAccelerations() {
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

void Simulation::updateAccelerations(std::vector<double>& serialBodies) {
  for (size_t i = 0; i < this->bodies.size(); ++i) {
    Body& body = this->bodies[i];
    if (!body.isActive()) {
      continue;  // Skip inactive bodies
    }
    // Reset acceleration to zero before recalculating
    body.resetAcceleration();
    for (size_t offset = 1; offset < serialBodies.size();
        offset += BODY_ACCELERATION_DATA_SIZE) {
      std::vector<double> auxPosition =
        {serialBodies[offset + ACCELERATION_POSITION_X],
          serialBodies[offset + ACCELERATION_POSITION_Y],
          serialBodies[offset + ACCELERATION_POSITION_Z]};

      body.updateAcceleration(serialBodies[offset + ACCELERATION_MASS],
        RealVector(auxPosition));
    }
  }
}

void Simulation::updateVelocities() {
  this->updateAccelerations();
  for (Body& body : this->bodies) {
    body.updateVelocity(this->deltaTime);
  }
}

void Simulation::updatePositions() {
  for (Body& body : this->bodies) {
    body.updatePosition(this->deltaTime);
  }
}

RealVector Simulation::distanceMean(std::vector<double>& serialBodies) {
  RealVector totalDistance = RealVector(DIM);
  for (size_t i = 0; i < this->bodies.size(); ++i) {
    for (size_t offset = 1; offset < serialBodies.size();
        offset += BODY_DISTANCE_DATA_SIZE) {
      std::vector<double> auxPosition = {serialBodies[offset],
        serialBodies[offset + 1], serialBodies[offset + 2]};
      if (this->bodies[i].isActive()) {
        RealVector distance = this->bodies[i].getPosition() -
          RealVector(auxPosition);
        totalDistance = totalDistance + distance;
      }
    }
  }
  return (totalDistance * (1.0 / this->activeBodiesCount));
}


RealVector Simulation::distanceMean() {
  RealVector totalDistance = RealVector(DIM);
  for (size_t i = 0; i < this->bodies.size() - 1; ++i) {
    for (size_t j = i + 1; j < this->bodies.size(); ++j) {
      if (this->bodies[i].isActive() && this->bodies[j].isActive()) {
        RealVector distance = this->bodies[i].getPosition() -
            this->bodies[j].getPosition();
        totalDistance = totalDistance + distance;
      }
    }
  }
  return (totalDistance * (1.0 / this->activeBodiesCount));
}

RealVector Simulation::distanceStdev(RealVector& mean) {
  RealVector totalDistance = RealVector(DIM);
  for (size_t i = 0; i < this->bodies.size() - 1; ++i) {
    for (size_t j = i + 1; j < this->bodies.size(); ++j) {
      if (this->bodies[i].isActive() && this->bodies[j].isActive()) {
        RealVector distance = this->bodies[i].getPosition() -
          this->bodies[j].getPosition();
        totalDistance = totalDistance + (distance - mean).pow(2);
      }
    }
  }
  return (totalDistance * (1.0 / this->activeBodiesCount)).pow(0.5);
}

RealVector Simulation::distanceStdev(std::vector<double>& serialBodies) {
  RealVector mean = this->distanceMean(serialBodies);
  RealVector totalDistance = RealVector(DIM);
  for (size_t i = 0; i < this->bodies.size(); ++i) {
    for (size_t offset = 1; offset < serialBodies.size();
        offset += BODY_DISTANCE_DATA_SIZE) {
      std::vector<double> auxPosition = {serialBodies[offset],
        serialBodies[offset + 1], serialBodies[offset + 2]};
      if (this->bodies[i].isActive()) {
        RealVector distance = this->bodies[i].getPosition() -
          RealVector(auxPosition);
        totalDistance = totalDistance + (distance - mean).pow(2);
      }
    }
  }
  return (totalDistance * (1.0 / this->activeBodiesCount)).pow(0.5);
}

RealVector Simulation::velocityMean() {
  RealVector totalVelocity = RealVector(DIM);
  for (Body& body : this->bodies) {
    if (body.isActive()) {
      totalVelocity = totalVelocity + body.getVelocity();
    }
  }
  return totalVelocity * (1.0 / this->activeBodiesCount);
}

RealVector Simulation::velocityMean(std::vector<double>& serialBodies) {
  RealVector totalVelocity = RealVector(DIM);
  for (size_t offset = 1; offset < serialBodies.size();
      offset += BODY_VELOCITY_DATA_SIZE) {
    std::vector<double> auxVelocity = {serialBodies[offset],
      serialBodies[offset + 1], serialBodies[offset + 2]};
    totalVelocity = totalVelocity + RealVector(auxVelocity);
  }
  return totalVelocity * (1.0 / this->activeBodiesCount);
}

RealVector Simulation::velocityStdev(RealVector& mean) {
  RealVector totalVelocity = RealVector(DIM);
  for (Body& body : this->bodies) {
    if (body.isActive()) {
      RealVector velocity = body.getVelocity();
      totalVelocity = totalVelocity + (velocity - mean).pow(2);
    }
  }
  return (totalVelocity * (1.0 / this->activeBodiesCount)).pow(0.5);
}

RealVector Simulation::velocityStdev(std::vector<double>& serialBodies) {
  RealVector mean = this->velocityMean(serialBodies);
  RealVector totalVelocity = RealVector(DIM);
  for (size_t offset = 1; offset < serialBodies.size();
      offset += BODY_VELOCITY_DATA_SIZE) {
    std::vector<double> auxVelocity = {serialBodies[offset],
      serialBodies[offset + 1], serialBodies[offset + 2]};
    RealVector velocity = RealVector(auxVelocity);
    totalVelocity = totalVelocity + (velocity - mean).pow(2);
  }
  return RealVector((totalVelocity * (1.0 / this->activeBodiesCount)).pow(0.5));
}

size_t Simulation::remaining() const {
  return this->activeBodiesCount;
}

void Simulation::saveBodiesFile(double currentTime) const {
  // remove the file extension from the universe file
  std::string fileName = this->universeFile.substr(0,
    this->universeFile.find_last_of('.'));
  fileName += "-" +std::to_string(static_cast<int>(currentTime)) + ".tsv";
  // TODO(any) Adjust for distribution, append if rank != 0
  std::ofstream file(fileName/*, std::ios::app*/);
  if (!file) {
    std::cerr << "Cannot open file for writing: " << fileName << std::endl;
    throw std::runtime_error("cannot save bodies file");
  }

  file << this->totalBodyCount << std::endl;
  for (const Body& body : this->bodies) {
    file << body << std::endl;
  }
  file.close();
}

void Simulation::reportResults() {
  printf("Simulation finished with %zu active bodies.\n",
         this->activeBodiesCount);
  RealVector meanDistance = this->distanceMean();
  RealVector meanVelocity = this->velocityMean();
  printf("Mean distance: %s\n", meanDistance.to_string().c_str());
  printf("Mean velocity: %s\n", meanVelocity.to_string().c_str());
  printf("Distance standard deviation: %s\n",
  this->distanceStdev(meanDistance).to_string().c_str());
  printf("Velocity standard deviation: %s\n",
  this->velocityStdev(meanVelocity).to_string().c_str());
}

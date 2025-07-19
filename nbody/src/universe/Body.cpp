// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "Body.hpp"

#include <cmath>
#include <sstream>
#include <string>

// Constructor - initializes body properties
Body::Body(const double mass, const double radius, const RealVector& position,
    const RealVector& velocity):
  mass(mass), radius(radius), position(position), velocity(velocity),
    acceleration() {
}

// Combines two radii using volume conservation (r^3 addition)
double Body::addRadiuses(double otherRadius) {
  return std::pow((std::pow(this->radius, 3) + std::pow(otherRadius, 3)),
    1.0 / 3);
}

// Updates acceleration based on gravitational force from another body
void Body::updateAcceleration(double otherMass,
    const RealVector& otherPosition) {
  RealVector distance = otherPosition - this->position;
  double distanceMagnitude = distance.getMagnitude();
  if (distanceMagnitude == 0) {  // Avoid division by zero
    return;
  }
  // Newton's law of universal gravitation: F = G*(m1*m2)/r^2
  this->acceleration = this->acceleration + distance *
    (otherMass / std::pow(distanceMagnitude, 3));
}

// Overloaded version that takes another Body object
void Body::updateAcceleration(const Body& other) {
  this->updateAcceleration(other.mass, other.position);
}

// Resets acceleration vector to zero
void Body::resetAcceleration() {
  this->acceleration = this->acceleration * 0;
}

// Updates velocity based on current acceleration and time step
void Body::updateVelocity(double deltaTime) {
  // v = v0 + a*t (with gravitational constant G factored in)
  this->velocity = this->velocity + (this->acceleration * G) * deltaTime;
}

// Updates position based on current velocity and time step
void Body::updatePosition(double deltaTime) {
  // x = x0 + v*t
  this->position = this->position + this->velocity * deltaTime;
}

// Checks if this body collides with another body given its radius and position
bool Body::checkCollision(double otherRadius,
    const RealVector& otherPosition) {
  // Collision occurs if distance between centers < sum of radii
  RealVector distance = otherPosition - this->position;
  if (distance.getMagnitude() < this->radius + otherRadius) {
    return true;
  }
  return false;
}

// Overloaded version that takes another Body object
bool Body::checkCollision(const Body& other) {
  return this->checkCollision(other.radius, other.position);
}

// Returns true if body is active (mass > 0)
bool Body::isActive() const {
  return this->mass > 0;
}

// Merges velocities during collision using momentum conservation
void Body::mergeVelocities(double otherMass, const RealVector& otherVelocity) {
  // p = m1*v1 + m2*v2
  this->velocity = this->velocity * otherMass + otherVelocity * otherMass;
  // v_combined = p / (m1 + m2)
  this->velocity = this->velocity * (1 / (this->mass + otherMass));
}

// Absorbs another body (combines mass, radius, and velocity)
// Returns true if this body absorbed the other (this mass >= other mass)
bool Body::absorb(double otherMass, double otherRadius,
  const RealVector& otherVelocity) {
  if (this->mass >= otherMass) {
    this->mass += otherMass;
    this->radius = this->addRadiuses(otherRadius);
    this->mergeVelocities(otherMass, otherVelocity);
    return true;
  }
  return false;
}

// Overloaded version that takes another Body object and deactivates it
bool Body::absorb(Body& other) {
  if (this->absorb(other.mass, other.radius, other.velocity)) {
    other.deactivate();  // Mark other body as inactive
    return true;
  }
  return false;
}

// Deactivates body by making mass negative
void Body::deactivate() {
  this->mass *= -1;
}

// Equality comparison operator
bool Body::operator==(const Body& other) const {
  return this->isEqualTo(other.mass, other.radius, other.position);
}

// Compares masses only
bool Body::equalMasses(const double otherMass) const {
  return this->mass == otherMass;
}

// Inequality comparison operator
bool Body::operator!=(const Body& other) const {
  return !(*this == other);
}

// Less-than comparison (by mass)
bool Body::operator<(const Body& other) const {
  return this->mass < other.mass;
}

// Greater-than comparison (by mass)
bool Body::operator>(const Body& other) const {
  return this->mass > other.mass;
}

// Detailed equality comparison of all properties
bool Body::isEqualTo(const double otherMass, const double otherRadius,
    const RealVector& otherPosition) const {
  return this->mass == otherMass && this->radius == otherRadius &&
      this->position == otherPosition;
}

// Detailed inequality comparison of all properties
bool Body::isNotEqualTo(const double otherMass, const double otherRadius,
    const RealVector& otherPosition) const {
  return !this->isEqualTo(otherMass, otherRadius, otherPosition);
}

// Serializes body data for collision checking (mass,radius,position,velocity)
void Body::serializeCheckCollision(std::vector<double>& serialized) const {
  serialized.push_back(this->mass);
  serialized.push_back(this->radius);
  this->serializePositionData(serialized);
  this->serializeVelocityData(serialized);
}

// Serializes body data for acceleration calculations (mass and position)
void Body::serializeAccelerationData(std::vector<double>& serialized) const {
  serialized.push_back(this->mass);
  this->serializePositionData(serialized);
}

// Serializes position data only
void Body::serializePositionData(std::vector<double>& serialized) const {
  serialized.push_back(this->position.x);
  serialized.push_back(this->position.y);
  serialized.push_back(this->position.z);
}

// Returns position vector
const RealVector& Body::getPosition() const {
  return this->position;
}

// Serializes velocity data only
void Body::serializeVelocityData(std::vector<double>& serialized) const {
  serialized.push_back(this->velocity.x);
  serialized.push_back(this->velocity.y);
  serialized.push_back(this->velocity.z);
}

// Returns velocity vector
const RealVector& Body::getVelocity() const {
  return this->velocity;
}

// Returns formatted string representation of body properties
std::string Body::toString() {
  std::stringstream bodyStream;
  bodyStream << "\nMass: " << std::to_string(this->mass) <<
    "\nRadius: " << std::to_string(this->radius) <<
    "\nPosition: " << this->position.toString() <<
    "\nVelocity: " << this->velocity.toString();
  return bodyStream.str();
}

// Output stream operator for writing body data
std::ostream& operator<<(std::ostream& output, const Body& body) {
  std::stringstream vectorsStream;
  vectorsStream << std::defaultfloat
      << body.position.x << "\t" << body.position.y << "\t"
      << body.position.z << "\t"
      << body.velocity.x << "\t" << body.velocity.y << "\t"
      << body.velocity.z;
  output << body.mass << "\t" << body.radius << "\t" << vectorsStream.str();
  return output;
}

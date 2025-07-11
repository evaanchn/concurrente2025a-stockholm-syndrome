// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "Body.hpp"
#include <cmath>
#include <vector>

Body::Body(double mass, double radius, RealVector position,
    RealVector velocity)
  : mass(mass), radius(radius), position(position), velocity(velocity),
    acceleration(RealVector(DIM)) {
}

double Body::addRadiuses(double otherRadius) {
  return std::pow((std::pow(this->radius, 3) + std::pow(otherRadius, 3)), 1/3);
}

void Body::updateAcceleration(double otherMass, RealVector otherPosition) {
  RealVector distance = otherPosition - this->position;
  double distanceMagnitude = distance.get_magnitude();
  if (distanceMagnitude == 0) {
    return;
  }
  this->acceleration = this->acceleration + distance *
    (otherMass / std::pow(distanceMagnitude, 3));
}

void Body::updateAcceleration(const Body& other) {
  this->updateAcceleration(other.mass, other.position);
}

void Body::resetAcceleration() {
  this->acceleration * 0;
}

void Body::updateVelocity(double deltaTime) {
  this->velocity = this->velocity + this->acceleration * deltaTime;
}

void Body::updatePosition(double deltaTime) {
  this->position = this->position + this->velocity * deltaTime;
}

bool Body::checkCollision(double otherRadius,
    RealVector otherPosition) {
  RealVector distance = otherPosition - this->position;
  if (distance.get_magnitude() < this->radius + otherRadius) {
    return true;
  }
  return false;
}

bool Body::checkCollision(const Body& other) {
  return this->checkCollision(other.radius, other.position);
}

void Body::deactivate() {
  this->mass *= -1;
}

bool Body::isActive() const {
  return this->mass > 0;
}

void Body::mergeVelocities(double otherMass, RealVector otherVelocity) {
  this->velocity = this->velocity * otherMass + otherVelocity * otherMass;
  this->velocity = velocity * (1 / (this->mass + otherMass));
}

bool Body::absorb(double otherMass, double otherRadius,
  RealVector otherVelocity) {
  if (this->mass >= otherMass) {
    this->mass += otherMass;
    this->addRadiuses(otherRadius);
    this->mergeVelocities(otherMass, otherVelocity);
    return true;
  }
  // this->deactivate();
  return false;
}

bool Body::absorb(Body& other) {
  if (this->absorb(other.mass, other.radius, other.velocity)) {
    other.deactivate();
    return true;
  }
  return false;
}

bool Body::operator==(const Body& other) const {
  return this->isEqualTo(other.mass, other.radius, other.position);
}

bool Body::operator!=(const Body& other) const {
  return !(*this == other);
}

bool Body::operator<(const Body& other) const {
  return this->mass < other.mass;
}

bool Body::operator>(const Body& other) const {
  return this->mass > other.mass;
}

bool Body::isEqualTo(const double otherMass, const double otherRadius,
    const RealVector otherPosition) const {
  return this->mass == otherMass && this->radius == otherRadius &&
      this->position == otherPosition;
}

bool Body::isNotEqualTo(const double otherMass, const double otherRadius,
    const RealVector otherPosition) const {
  return !this->isEqualTo(otherMass, otherRadius, otherPosition);
}

std::vector<double> Body::serializeCheckCollision() const {
  return {this->mass, this->radius, this->position[0], this->position[1],
    this->position[2], this->velocity[0], this->velocity[1], this->velocity[2]};
}

std::vector<double> Body::serializeAccelerationData() const {
  return {this->mass, this->position[0], this->position[1], this->position[2]};
}

std::vector<double> Body::serializePositionData() const {
  return {this->position[0], this->position[1], this->position[2]};
}

RealVector Body::getPosition() const {
  return this->position;
}

std::vector<double> Body::serializeVelocityData() const {
  return {this->velocity[0], this->velocity[1], this->velocity[2]};
}

RealVector Body::getVelocity() const {
  return this->velocity;
}

// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "RealVector.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

// Vector addition - adds corresponding components
RealVector RealVector::operator+(const RealVector& other) const {
  // Sum corresponding components
  double x = this->x + other.x;
  double y = this->y + other.y;
  double z = this->z + other.z;
  // Return a new RealVector containing the result
  return RealVector(x, y, z);  // Return new vector with result
}

RealVector RealVector::operator-(const RealVector& other) const {
  // Use the addition operator to perform subtraction
  return *this + other * -1;
}

RealVector RealVector::operator*(const RealVector& other) const {
  // Multiply corresponding components
  double productX = this->x * other.x;
  double productY = this->y * other.y;
  double productZ = this->z * other.z;
  // Return new vector with result
  return RealVector(productX, productY, productZ);
}

RealVector RealVector::operator*(double scalar) const {
  double productX = this->x * scalar;  // Scale x component
  double productY = this->y * scalar;  // Scale y component
  double productZ = this->z * scalar;  // Scale z component
  // Return new vector with scaled components
  return RealVector(productX, productY, productZ);
}

// Raises each component to the given power
RealVector RealVector::pow(double exponent) {
  this->x = std::pow(this->x, exponent);  // Raise x to power
  this->y = std::pow(this->y, exponent);  // Raise y to
  this->z = std::pow(this->z, exponent);  // Raise z to power
  return *this;  // Return reference to modified vector
}

// Equality comparison - checks if all components are equal
bool RealVector::operator==(const RealVector& other) const {
  // Compare each corresponding component
  return this->x == other.x &&  // Check x components
         this->y == other.y &&  // Check y components
         this->z == other.z;     // Check z components
}

// Inequality comparison - opposite of equality
bool RealVector::operator!=(const RealVector& other) const {
  return !(*this == other);  // Negate the equality check
}

// Returns a string representation of the vector
std::string RealVector::toString() const {
  std::stringstream vectorStream;
  vectorStream << "<";  // Start with opening angle bracket
  // Add each component to the string
  vectorStream << std::defaultfloat << this->x << ", " <<
    std::defaultfloat << this->y << ", " <<
    std::defaultfloat << this->z << ">";
  return vectorStream.str();  // Convert stream to string
}

// Calculates the Euclidean norm (magnitude) of the vector
double RealVector::getMagnitude() const {
  double sum = this->x * this->x +  // Square x component
               this->y * this->y +  // Square y component
               this->z * this->z;   // Square z component
  return std::sqrt(sum);  // Return square root of sum of squares
}

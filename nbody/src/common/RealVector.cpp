// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "RealVector.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

// Returns the number of components in the vector
size_t RealVector::size() const {
    return this->components.size();
}

// Vector addition - adds corresponding components
RealVector RealVector::operator+(const RealVector& other) const {
  assert(this->size() == other.size());  // Vectors must be same size
  std::vector<double> result;
  result.reserve(this->components.size());  // Pre-allocate memory
  // Append the sums to the new vector
  for (size_t i = 0; i < this->components.size(); ++i) {
    result.push_back(this->components[i] + other.components[i]);
  }
  // Return a new RealVector containing the result
  return RealVector(result);  // Return new vector with result
}

RealVector RealVector::operator-(const RealVector& other) const {
  assert(this->size() == other.size());  // Vectors must be same size
  // Use the addition operator to perform subtraction
  return *this + other * -1;
}

RealVector RealVector::operator*(const RealVector& other) const {
  assert(this->size() == other.size());  // Vectors must be same size
  std::vector<double> result;
  result.reserve(this->components.size());  // Pre-allocate memory
  // Multiply corresponding components
  for (size_t i = 0; i < this->components.size(); ++i) {
    result.push_back(this->components[i] * other.components[i]);
  }
  return RealVector(result);  // Return new vector with result
}

RealVector RealVector::operator*(double scalar) const {
  // Create copy of current vector
  RealVector result = RealVector(this->components);
  // Multiply each component by the scalar
  for (double& component : result.components) {
    component = component * scalar;
  }
  return result;
}

// Raises each component to the given power
RealVector RealVector::pow(double exponent) {
  for (double& component : this->components) {
    component = std::pow(component, exponent);
  }
  return *this;  // Return reference to modified vector
}

// Accessor for vector components
double RealVector::operator[](size_t index) {
  assert(index < this->size());  // Index must be valid
  return this->components[index];
}

// Accessor for vector components (const version)
double RealVector::operator[](size_t index) const {
  assert(index < this->size());  // Index must be valid
  return this->components[index];
}

// Equality comparison - checks if all components are equal
bool RealVector::operator==(const RealVector& other) const {
  assert(this->size() == other.size());  // Vectors must be same size
  // Compare each corresponding component
  for (size_t i = 0; i < this->components.size(); ++i) {
    if (this->components[i] != other.components[i]) {
      return false;  // Found unequal components
    }
  }
  return true;  // All components equal
}

// Inequality comparison - opposite of equality
bool RealVector::operator!=(const RealVector& other) const {
  assert(this->size() == other.size());  // Vectors must be same size
  return !(*this == other);  // Negate the equality check
}

// Returns a string representation of the vector
std::string RealVector::toString() const {
  std::stringstream vectorStream;
  vectorStream << "<";  // Start with opening angle bracket
  // Add each component to the string
  for (size_t i = 0; i < this->components.size(); ++i) {
    vectorStream /*<< std::setprecision(8)*/ << std::defaultfloat
      << this->components[i];  // Add component value
    // Add comma separator if not last component
    if (i < this->components.size() - 1) {
      vectorStream << ", ";
    }
  }
  vectorStream << ">";  // Close with angle bracket
  return std::string(vectorStream.str());  // Convert stream to string
}

// Calculates the Euclidean norm (magnitude) of the vector
double RealVector::getMagnitude() const {
  double sum = 0;
  // Sum the squares of all components
  for (const auto& component : this->components) {
    sum += component * component;
  }
  return std::sqrt(sum);  // Return square root of sum of squares
}

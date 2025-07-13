// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "RealVector.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

size_t RealVector::size() const {
    return this->components.size();
}

RealVector RealVector::operator+(const RealVector& other) const {
  assert(this->size() == other.size());
  std::vector<double> result;
  result.reserve(this->components.size());
  // Append the sums to the new vector
  for (size_t i = 0; i < this->components.size(); ++i) {
    result.push_back(this->components[i] + other.components[i]);
  }
  // Return a new RealVector containing the result
  return RealVector(result);
}

RealVector RealVector::operator-(const RealVector& other) const {
  assert(this->size() == other.size());
  // Use the addition operator to perform subtraction
  return *this + other * -1;
}

RealVector RealVector::operator*(const RealVector& other) const {
  assert(this->size() == other.size());
  std::vector<double> result;
  result.reserve(this->components.size());
  for (size_t i = 0; i < this->components.size(); ++i) {
    result.push_back(this->components[i] * other.components[i]);
  }
  return RealVector(result);
}

RealVector RealVector::operator*(double scalar) const {
  RealVector result = RealVector(this->components);
  for (double& component : result.components) {
    component = component * scalar;
  }
  return result;
}

RealVector RealVector::pow(double exponent) {
  for (double& component : this->components) {
    component = std::pow(component, exponent);
  }
  return *this;
}

double RealVector::operator[](size_t index) {
  assert(index < this->size());
  return this->components[index];
}

double RealVector::operator[](size_t index) const {
  assert(index < this->size());
  return this->components[index];
}

bool RealVector::operator==(const RealVector& other) const {
  assert(this->size() == other.size());
  for (size_t i = 0; i < this->components.size(); ++i) {
    if (this->components[i] != other.components[i]) {
      return false;
    }
  }
  return true;
}

bool RealVector::operator!=(const RealVector& other) const {
  assert(this->size() == other.size());
  return !(*this == other);
}

std::string RealVector::toString() const {
  std::stringstream vectorStream;
  vectorStream << "<";
  for (size_t i = 0; i < this->components.size(); ++i) {
    vectorStream /*<< std::setprecision(8)*/ << std::defaultfloat
      << this->components[i];
    if (i < this->components.size() - 1) {
      vectorStream << ", ";
    }
  }
  vectorStream << ">";
  return std::string(vectorStream.str());
}

double RealVector::getMagnitude() const {
  double sum = 0;
  for (const auto& component : this->components) {
    sum += component * component;
  }
  return std::sqrt(sum);
}

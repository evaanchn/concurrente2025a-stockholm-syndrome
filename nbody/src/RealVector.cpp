#include <cmath>

#include "RealVector.hpp"

size_t RealVector::size() const {
    return this->components.size();
}

RealVector RealVector::operator+(const RealVector& other) {
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

RealVector RealVector::operator-(const RealVector& other) {
  assert(this->size() == other.size());
  // Copy other vector to negate with scalar multiplication
  RealVector otherNegative = other;
  // Use the addition operator to perform subtraction
  return *this + otherNegative * -1;
}

RealVector RealVector::operator*(const RealVector& other) {
  assert(this->size() == other.size());
  std::vector<double> result;
  result.reserve(this->components.size());
  for (size_t i = 0; i < this->components.size(); ++i) {
    result.push_back(this->components[i] * other.components[i]);
  }
  return RealVector(result);
}

bool RealVector::operator==(const RealVector& other) {
  assert(this->size() == other.size());
  for (size_t i = 0; i < this->components.size(); ++i) {
    if (this->components[i] != other.components[i]) {
      return false;
    }
  }
  return true;
}

bool RealVector::operator!=(const RealVector& other) {
  assert(this->size() == other.size());
  return !(*this == other);
}

RealVector RealVector::operator*(double scalar) {
  for (double& component : this->components) {
    component = component * scalar;
  }
  return *this;
}

RealVector RealVector::pow(int exponent) {
  for (double& component : this->components) {
    component = std::pow(component, exponent);
  }
  return *this;
}

std::string RealVector::to_string() {
  std::string vectorStr = "<";
  for (size_t i = 0; i < this->components.size(); ++i) {
    vectorStr += std::to_string(this->components[i]);
    if (i < this->components.size() - 1) {
      vectorStr += ", ";
    }
  }
  vectorStr += ">";
  return vectorStr;
}

double RealVector::get_magnitude() const {
  double sum = 0;
  for (const auto& component : this->components) {
    sum += component * component;
  }
  return std::sqrt(sum);
}

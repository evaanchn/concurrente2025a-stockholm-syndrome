// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef REALVECTOR_HPP
#define REALVECTOR_HPP

#include <iostream>
#include <cassert>
#include <string>
#include <vector>

/// @brief Struct representing a mathematical vector, with real values
struct RealVector {
 public:
  /// x component
  double x = 0.0;
  /// y component
  double y = 0.0;
  /// z component
  double z = 0.0;

 public:
  /// @brief Default constructor for RealVector
  RealVector() = default;

  /// @brief Constructor for RealVector with specified components
  /// @param x X component of the vector
  /// @param y Y component of the vector
  /// @param z Z component of the vector
  RealVector(double x, double y, double z) :
    x(x), y(y), z(z) {
  }

  /// @brief Constructor for RealVector using vector
  /// @param components Vector with x y z components
  explicit RealVector(const std::vector<double>& components) {
    assert(components.size() == 3);
    this->x = components[0];
    this->y = components[1];
    this->z = components[2];
  }

 public:  // Operators
  /// @brief Vector addition
  /// @param other The other vector to operate with
  /// @return Result of operation
  RealVector operator+(const RealVector& other) const;
  /// @brief Vector subtraction
  /// @see operator+
  RealVector operator-(const RealVector& other) const;
  /// @brief Vector multiplication
  /// @see operator+
  RealVector operator*(const RealVector& other) const;
  /// @brief Vector-scalar multiplication
  /// @see operator+
  RealVector operator*(double scalar) const;
  /// @brief Components comparison
  /// @return True if all components are the same, false otherwise
  bool operator==(const RealVector& other) const;
  /// @see operator==
  bool operator!=(const RealVector& other) const;

 public:
  /// @brief Elevates each component of the vector to the specified exponent
  /// @param exponent Power to elevate to
  /// @return this, RealVector after applying exponents
  RealVector pow(double exponent);
  /// @brief Calculate norm, or square root of the sum of each component squared
  /// @return Calculated magnitude
  double getMagnitude() const;

 public:
  /// @brief Converts the vector to a string representation
  std::string toString() const;
};

#endif  // REALVECTOR_HPP

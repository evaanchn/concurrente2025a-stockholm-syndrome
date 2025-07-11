// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <vector>
#include <iostream>
#include <cassert>
#include <string>

class RealVector {
 private:
  std::vector<double> components;  /// Container of elements

 public:  // Constructors
  RealVector() = default;

  explicit RealVector(std::vector<double>& components) :
    components(components) {
  }

  explicit RealVector(size_t count) :
    components(count, double()) {
  }

 public:
  /// @brief Returns the size of the real vector.
  /// @return The size of the vector.
  size_t size() const;

 public:  // Operators
  /// @brief Vector addition
  /// @param other The other vector to operate with
  /// @return Result of operation
  RealVector operator+(const RealVector& other);
  /// @brief Vector subtraction
  /// @see operator+
  RealVector operator-(const RealVector& other);
  /// @brief Vector multiplication
  /// @see operator+
  RealVector operator*(const RealVector& other);
  /// @brief Components comparison
  /// @return True if all components are the same, false otherwise
  bool operator==(const RealVector& other) const;
  /// @see operator==
  bool operator!=(const RealVector& other) const;
  /// @brief Vector-scalar multiplication
  /// @see operator+
  RealVector operator*(double scalar);
  /// @brief Access to vector components, modifiable
  double operator[](size_t index);
  /// @brief Access to vector components, not modifiable
  double operator[](size_t index) const;

 public:
  /// @brief Elevates each component of the vector to the specified exponent
  /// @param exponent Power to elevate to
  /// @return this, RealVector after applying exponents
  RealVector pow(double exponent);
  /// @brief Calculate norm, or square root of the sum of each component squared
  /// @return Calculated magnitude
  double get_magnitude() const;

 public:
  /// @brief Converts the vector to a string representation
  std::string to_string();
};

#endif  // VECTOR_HPP

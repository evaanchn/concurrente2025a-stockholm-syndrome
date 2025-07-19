// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef BODY_HPP
#define BODY_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "RealVector.hpp"

const double G = 6.67e-11;  // Gravitational constant
#define DIM 3

/// @brief Class representing a body the universe
/// @details Contains the properties of a body, such as mass, radius, position,
// velocity, and acceleration.
class Body {
 private:
  /// Body's mass
  double mass;
  /// Body's radius
  double radius;
  /// Body's position in space
  RealVector position;
  /// Body's velocity in space
  RealVector velocity;
  /// Body's acceleration in space
  RealVector acceleration;

 public:
  /// @brief Constructor for the Body class.
  Body(const double mass, const double radius, const RealVector& position,
    const RealVector& velocity);

 public:
  /// @brief Update the acceleration of the body, with separated mass and pos
  /// @param otherMass Mass of the other body
  /// @param otherPosition Position of the other body
  /// @details This method calculates the gravitational acceleration
  void updateAcceleration(double otherMass, const RealVector& otherPosition);

  /// @brief update the acceleration of the body, with data from another body
  /// @param other Body to update acceleration from
  /// @see updateAcceleration(double otherMass, RealVector otherPosition)
  void updateAcceleration(const Body& other);

  /// @brief Reset acceleration to a vector of zeroes to prepare for new update
  void resetAcceleration();

  /// @brief Update the velocity of the body
  /// @param deltaTime Time step for the update
  void updateVelocity(double deltaTime);

  /// @brief update the position of the body
  /// @param deltaTime Time step for the update
  /// @details This method updates the position based on the velocity
  void updatePosition(double deltaTime);

  /// @brief check the collision between two bodies
  /// @param otherRadius Radius of the other body
  /// @param otherPosition Position of the other body
  /// @return true if the bodies collide, false otherwise
  bool checkCollision(double otherRadius,
    const RealVector& otherPosition);

  /// @brief check the collision between two bodies
  /// @param other Body to check collision with
  /// @see checkCollision(double otherRadius, RealVector otherPosition)
  bool checkCollision(const Body& other);

  /// @brief represents the absorption of another body
  /// @param otherMass Mass of the other body
  /// @param otherRadius Radius of the other body
  /// @param otherVelocity Velocity of the other body
  bool absorb(double otherMass, double otherRadius,
      const RealVector& otherVelocity);

  /// @brief represents the absorption of another body
  /// @param other Body to absorb
  /// @see absorb(double otherMass, double otherRadius,
  /// RealVector otherVelocity)
  bool absorb(Body& other);
  /// @brief deactivates the body, making it inactive in the simulation
  void deactivate();

 private:  // Auxiliary methods for collision
  /// @brief merge the radius of two bodies and calculate the new radius
  /// @param otherRadius Radius of the other body
  /// @return the new radius after merging
  double addRadiuses(double otherRadius);

  /// @brief merge the velocity of two bodies and calculate the new velocity
  /// @param otherMass Mass of the other body
  /// @param otherVelocity Velocity of the other body
  void mergeVelocities(double otherMass, const RealVector& otherVelocity);

 public:  // Comparison methods
  /// @brief check if two bodies are equal based on their properties
  /// @param other Body to compare with
  /// @see isEqualTo(double otherMass, double otherRadius,
  /// RealVector otherPosition)
  bool operator==(const Body& other) const;

  /// @brief check if the body is equal based on its mass
  /// @param otherMass Mass of the other body
  /// @return true if the body is equal, false otherwise
  bool equalMasses(const double otherMass) const;

  /// @brief check if two bodies are not equal based on their properties
  /// @param other Body to compare with
  /// @see isNotEqualTo(double otherMass, double otherRadius,
  /// RealVector otherPosition)
  bool operator!=(const Body& other) const;

  /// @brief compare two bodies based on their mass
  bool operator<(const Body& other) const;

  /// @brief compare two bodies based on their mass
  bool operator>(const Body& other) const;

  /// @brief check if two bodies are equal based on their properties
  /// @param otherMass Mass of the other body
  /// @param otherRadius Radius of the other body
  /// @param otherPosition Position of the other body
  /// @return true if the bodies are equal, false otherwise
  bool isEqualTo(const double otherMass, const double otherRadius,
    const RealVector& otherPosition) const;

  /// @brief check if two bodies are not equal based on their properties
  /// @param otherMass Mass of the other body
  /// @param otherRadius Radius of the other body
  /// @param otherPosition Position of the other body
  /// @return true if the bodies are not equal, false otherwise
  bool isNotEqualTo(const double otherMass, const double otherRadius,
    const RealVector& otherPosition) const;

  /// @brief check if the body is active
  /// @return true if the body is active, false otherwise
  bool isActive() const;

  /// @brief serialize the body for collision checking
  /// @param serialized Vector to store the serialized data
  void serializeCheckCollision(std::vector<double>& serialized) const;

  /// @brief serialize the body for acceleration sum
  /// @param serialized Vector to store the serialized data
  void serializeAccelerationData(std::vector<double>& serialized) const;

  /// @brief serialize the body position
  /// @param serialized Vector to store the serialized data
  void serializePositionData(std::vector<double>& serialized) const;

  /// @brief serialize the body velocity
  /// @param serialized Vector to store the serialized data
  void serializeVelocityData(std::vector<double>& serialized) const;

  /// @brief Get the velocity of the body
  const RealVector& getVelocity() const;

  /// @brief Get the position of the body
  const RealVector& getPosition() const;

  /// @brief String representation of the body
  std::string toString();

  /// @brief Friend function to output the body properties
  friend std::ostream& operator<<(std::ostream& output, const Body& body);
};

/// @brief Overloaded output operator for Body class
std::ostream& operator<<(std::ostream& output, const Body& body);

#endif  // BODY_HPP

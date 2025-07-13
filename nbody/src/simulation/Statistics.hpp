// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include <vector>

#include "Body.hpp"
#include "common.hpp"
#include "RealVector.hpp"

// Forward declaration
class Mpi;

class Statistics {
  DISABLE_COPY(Statistics);

 public:
  Statistics() = default;
  ~Statistics() = default;

 public:  // MEAN
  /// @brief Calculates the average distance of the bodies
  RealVector realVectorMeanDistributed(Mpi* mpi,
      const std::vector<RealVector>& realVectors, const int total);

 private:  // MEAN HELPER
  /// @brief Calculate summation of RealVectors
  RealVector calculateVectorsSum(const std::vector<RealVector>& realVectors);

 public:  // STDEV
  /// @brief Calculate the standard deviation of the distances of the bodies.
  /// @return A RealVector representing the standard deviation of the distances.
  RealVector realVectorStDevDistributed(Mpi* mpi,
    const std::vector<RealVector>& realVectors, RealVector& mean,
    const int total);

 private:  // STDEV HELPER
  /// @brief Calculate the standard deviation of thes of the bodies.
  /// @param
  /// @return
  RealVector calculateStDevSum(const std::vector<RealVector>& realVectors,
      RealVector& mean);
};

#endif  // STATISTICS_HPP

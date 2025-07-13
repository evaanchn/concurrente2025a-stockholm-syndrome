// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include <vector>

#include "Body.hpp"
#include "common.hpp"
#include "RealVector.hpp"

// Forward declaration
class Mpi;

/**
 * @class Statistics
 * @brief Provides statistical calculations for N-body simulations
 * 
 * This class handles distributed computation of statistics like mean
 * and standard deviation across MPI processes.
 */
class Statistics {
  DISABLE_COPY(Statistics);

 public:
  Statistics() = default;
  ~Statistics() = default;

 public:  // MEAN
  /**
   * @brief Calculates the average of RealVectors across all MPI processes
   * @param mpi The MPI communication interface
   * @param realVectors The local vectors to include in calculation
   * @param total The total number of elements across all processes
   * @return RealVector containing the mean values
   */
  RealVector realVectorMeanDistributed(Mpi* mpi,
      const std::vector<RealVector>& realVectors, const int total);

 private:  // MEAN HELPER
  /**
   * @brief Calculates the sum of all vectors in the local process
   * @param realVectors The vectors to sum
   * @return RealVector containing the component-wise sum
   */
  RealVector calculateVectorsSum(const std::vector<RealVector>& realVectors);

 public:  // STDEV
  /**
   * @brief Calculates standard deviation across all MPI processes
   * @param mpi The MPI communication interface
   * @param realVectors The local vectors to include in calculation
   * @param mean The pre-calculated mean vector
   * @param total The total number of elements across all processes
   * @return RealVector containing the standard deviations
   */
  RealVector realVectorStDevDistributed(Mpi* mpi,
    const std::vector<RealVector>& realVectors, RealVector& mean,
    const int total);

 private:  // STDEV HELPER
  /**
   * @brief Calculates sum of squared differences from mean for local vectors
   * @param realVectors The vectors to process
   * @param mean The mean vector to compare against
   * @return RealVector containing sum of squared differences
   */
  RealVector calculateStDevSum(const std::vector<RealVector>& realVectors,
      RealVector& mean);
};

#endif  // STATISTICS_HPP

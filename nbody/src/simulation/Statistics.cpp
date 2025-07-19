// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include "Statistics.hpp"

#include <omp.h>
#include <vector>

#include "common.hpp"
#include "Mpi.hpp"
#include "RealVector.hpp"

// CREATED USING CHATGPT
// This declaration defines a custom reduction for RealVector
// The behavior for the reduction is omp_out = omp_out + omp_in
// The private reduction variable is defined with initializer, where
// It will start as a vector of DIM dimensions, full of zeroes
#pragma omp declare reduction(vec_sum : RealVector : omp_out = omp_out + \
    omp_in) initializer(omp_priv = RealVector())

RealVector Statistics::realVectorMeanDistributed(Mpi* mpi,
    const std::vector<RealVector>& realVectors, const int total) {
  // First calculate total sum of this process' vector of real vectors
  std::vector<double> localSum = Statistics::calculateVectorsSum(realVectors);
  // Prepare buffer for sum result
  std::vector<double> sumResult = std::vector<double>(DIM, 0.0);
  // Reduce the total sum with other process' sum
  mpi->allReduce(localSum, sumResult, MPI_SUM);
  // Divide each component by total
  return RealVector(sumResult) * (1.0 / total);
}

std::vector<double>  Statistics::calculateVectorsSum(
    const std::vector<RealVector>& realVectors) {
  // First create a 3D real vector with 0s
  RealVector sum = RealVector();
  // Parallel for to reduce the sum with custom reduction vec_sum
  #pragma omp parallel for num_threads(omp_get_max_threads()) \
    default(none) shared(realVectors) reduction(vec_sum:sum)
  for (size_t index = 0; index < realVectors.size(); ++index) {
    sum = sum + realVectors[index];
  }
  return {sum.x, sum.y, sum.z};
}

RealVector Statistics::realVectorStDevDistributed(Mpi* mpi,
    const std::vector<RealVector>& realVectors, RealVector& mean,
    const int total) {
  // If total is 1, no standard deviation can be calculated
  if (total <= 1) {
      return RealVector();
  }

  // Sum the (value - mean)^2
  std::vector<double> stDevSum = Statistics::calculateStDevSum(realVectors,
      mean);
  // Buffer for total sum result with other processes
  std::vector<double> sumResult = std::vector<double>(DIM,
      0.0);
  // Reduce the serialized stdev sum across all processes
  mpi->allReduce(stDevSum, sumResult, MPI_SUM);
  // Remove mean
  int degreesOfFreedom = total - 1;
  // Divide by degrees of freedom and square root
  return (RealVector(sumResult) * (1.0 / degreesOfFreedom)).pow(0.5);
}

std::vector<double>  Statistics::calculateStDevSum(
    const std::vector<RealVector>& realVectors, RealVector& mean) {
  // First create a real vector of 0s
  RealVector sum = RealVector();
  // Sum each (value - mean)^2
  // Parallel for to reduce the sum with vec_sum
  #pragma omp parallel for num_threads(omp_get_max_threads()) \
    default(none) shared(realVectors, mean) reduction(vec_sum:sum)
  for (size_t index = 0; index < realVectors.size(); ++index) {
    sum = sum + (realVectors[index] - mean).pow(2);
  }
  return {sum.x, sum.y, sum.z};
}

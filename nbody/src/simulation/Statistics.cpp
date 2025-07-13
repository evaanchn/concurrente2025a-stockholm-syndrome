// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0


#include <vector>

#include "Statistics.hpp"
#include "common.hpp"
#include "Mpi.hpp"
#include "RealVector.hpp"

RealVector Statistics::realVectorMeanDistributed(Mpi* mpi,
    const std::vector<RealVector>& realVectors, const int total) {
  // First calculate total sum of this process' vector of real vectors
  RealVector localSum = this->calculateVectorsSum(realVectors);
  // Prepare buffer for sum result
  std::vector<double> sumResult = std::vector<double>(localSum.size(),
    0.0);
  // Reduce the total sum with other process' sum
  mpi->allReduce(localSum.components, sumResult, MPI_SUM);
  // Divide each component by total
  return RealVector(sumResult) * (1.0 / total);
}

RealVector Statistics::calculateVectorsSum(const std::vector<RealVector>&
    realVectors) {
  // First create a 3D real vector with 0s
  RealVector sum = RealVector(DIM);
  // For each real vector in the collection sent, add to sum
  for (const RealVector& realVector : realVectors) {
    sum = sum + realVector;
  }
  return sum;
}

RealVector Statistics::realVectorStDevDistributed(Mpi* mpi,
    const std::vector<RealVector>& realVectors, RealVector& mean,
    const int total) {
  // If total is 1, no standard deviation can be calculated
  if (total <= 1) {
      return RealVector(DIM);
  }

  // Sum the (value - mean)^2
  RealVector stDevSum = this->calculateStDevSum(realVectors, mean);
  // Buffer for total sum result with other processes
  std::vector<double> sumResult = std::vector<double>(stDevSum.size(),
      0.0);
  // Reduce the serialized stdev sum across all processes
  mpi->allReduce(stDevSum.components, sumResult, MPI_SUM);
  // Remove mean
  int degreesOfFreedom = total - 1;
  // Divide by degrees of freedom and square root
  return (RealVector(sumResult) * (1.0 / degreesOfFreedom)).pow(0.5);
}

RealVector Statistics::calculateStDevSum(
    const std::vector<RealVector>& realVectors, RealVector& mean) {
  // First create a real vector of 0s
  RealVector sum = RealVector(DIM);
  // Sum each (value - mean)^2
  for (const RealVector& realVector : realVectors) {
    sum = sum + (realVector - mean).pow(2);
  }
  return sum;
}

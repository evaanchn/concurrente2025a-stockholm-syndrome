// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0

#include <iostream>

#include "RealVector.hpp"
#include "Simulation.hpp"

int main(int argc, char* argv[]) {
  try {
    Simulation simulation;
    simulation.run(argc, argv);
  } catch(const std::runtime_error& error) {
    std::cerr << error.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

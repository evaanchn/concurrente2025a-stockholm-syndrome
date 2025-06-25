// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
#ifndef CALCULATOR_HPP
#define CALCULATOR_HPP

#include "Assembler.hpp"
#include "DataUnit.hpp"

class Calculator : public Assembler<DataUnit*, DataUnit*> {
 public:
  Calculator() = default;
  /// @brief  Consume a DataUnit pointer with ref to an integer to be processed
  /// @param data DataUnit pointer to register with data to be processed
  /// @details This method processes the input data and produces a result
  /// by calling the AppCalculator's processNumber method.
  void consume(DataUnit* data) override;

  /// @brief Start consuming loop
  /// @return EXIT SUCCESS / EXIT FAILURE
  int run() override;
};

#endif  // CALCULATOR_HPP

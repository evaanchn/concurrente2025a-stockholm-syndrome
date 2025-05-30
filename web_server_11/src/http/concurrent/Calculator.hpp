// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
#ifndef CALCULATOR_HPP
#define CALCULATOR_HPP

#include <iostream>
#include <vector>
#include "Assembler.hpp"
#include "RequestUnit.hpp"
#include "RequestData.hpp"
#include "AppCalculator.hpp"

class Calculator : public Assembler<RequestUnit, RequestUnit> {
 public:
  Calculator() = default;
  /// @brief  Consume a RequestUnit containing an integer to be processed
  /// @param data Pointer to the RequestUnit containing the data to be processed
  /// @details This method processes the input data and produces a result
  /// by calling the AppCalculator's processNumber method.
  void consume(RequestUnit data) override;

  /// @brief Start consuming loop
  /// @return EXIT SUCCESS / EXIT FAILURE
  int run() override;
};

#endif  // CALCULATOR_HPP

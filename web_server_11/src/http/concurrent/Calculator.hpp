// Copyright 2025 Stockholm Syndrome. Universidad de Costa Rica. CC BY 4.0
#ifndef CALCULATOR_HPP
#define CALCULATOR_HPP

#include <iostream>
#include <vector>
#include "Assembler.hpp"
#include "RequestUnit.hpp"
#include "AppCalculator.hpp"

class Calculator : public Assembler<RequestUnit<int64_t, int64_t>*,
    RequestUnit<int64_t, int64_t>*> {
 public:
  Calculator() = default;
  void consume(RequestUnit<int64_t, int64_t> *data) override;
  int run() override;
};

#endif  // CALCULATOR_HPP

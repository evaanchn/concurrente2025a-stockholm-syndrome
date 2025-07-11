// Copyright 2025 stack_underflow CC-BY 4.0

#pragma once

#include <random>
#include <type_traits>

template <typename DataType>
class Random {
 private:
  std::random_device rd;
  std::mt19937 gen{rd()};

 public:
  Random() = default;

  /**
   * @brief Generates a random value within a specified range.
   *
   * This function generates a random number in interval [start, end]
   * It selects the appropriate random distribution depending on whether the
   * `DataType` is integral or floating-point.
   *
   * @tparam DataType The numeric type (e.g., int, float, double).
   * @param start The minimum value of the range (inclusive).
   * @param end The maximum value of the range (inclusive for integers).
   * @return A random number of type DataType within the specified range.
   *
   * @note This function uses SFINAE with `if constexpr` to statically select 
   * the appropriate distribution.
   * 
   * @throws A compile-time error if DataType is not an arithmetic type.
   */
  DataType between(DataType start, DataType end) {
    // Must check if is whole number or floating point
    if constexpr (std::is_integral_v<DataType>) {
      std::uniform_int_distribution<DataType> dist(start, end);
      return dist(gen);
    } else if constexpr (std::is_floating_point_v<DataType>) {
      std::uniform_real_distribution<DataType> dist(start, end);
      return dist(gen);
    } else {
      static_assert(std::is_arithmetic_v<DataType>
          , "DataType must be arithmetic (integral or floating point)");
    }
  }

  /**
   * @brief Randomly selects one of two values.
   *
   * This function returns either `option` or `alternative`
   * with equal probability.
   *
   * @tparam DataType The type of the values to choose from.
   * @param option The first option.
   * @param alternative The second option.
   * @return Either `option` or `alternative`, chosen at random.
   */
  DataType pick(DataType option, DataType alternative) {
    // Applies for whole number and floating point numbers with 0.5
    return between(0, 1) < 0.5 ? option : alternative;
  }
};

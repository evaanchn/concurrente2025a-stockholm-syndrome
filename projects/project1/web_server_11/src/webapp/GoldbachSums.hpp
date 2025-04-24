#ifndef GOLDBACHSUMS_HPP
#define GOLDBACHSUMS_HPP

#include <iostream>

class GoldbachSums {
public:
  void processNumber(int number);

private:
  bool isPrime(int n);

  bool strongGoldbach(int n);

  bool weakGoldbach(int n);
};

#endif

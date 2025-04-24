#include "GoldbachSums.hpp"

using namespace std;

void GoldbachSums::processNumber(int number) {
  if (number % 2 == 0) {
    if (!strongGoldbach(number))
      printf("No se encontró una suma válida de dos primos.");
  } else {
    if (!weakGoldbach(number))
      printf("No se encontró una suma válida de tres primos.");
  }
}

bool GoldbachSums::isPrime(int n) {
  if (n <= 1) return false;
  if (n == 2) return true;
  if (n % 2 == 0) return false;
  for (int i = 3; i * i <= n; i += 2)
    if (n % i == 0) return false;
  return true;
}

bool GoldbachSums::strongGoldbach(int n) {
  for (int i = 2; i <= n / 2; ++i) {
    if (isPrime(i) && isPrime(n - i)) {
      printf("%d = %d + %d",n,i,n-i);
      return true;
    }
  }
  return false;
}

bool GoldbachSums::weakGoldbach(int n) {
  for (int i = 2; i < n; ++i) {
    if (!isPrime(i)) continue;
    for (int j = 2; j < n - i; ++j) {
      if (!isPrime(j)) continue;
      int k = n - i - j;
      if (k >= 2 && isPrime(k)) {
        printf("%d = %d + %d + %d",n,i,j,k);
        return true;
      }
    }
  }
  return false;
}

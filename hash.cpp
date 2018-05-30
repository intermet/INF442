#include "hash.hpp"

int HashSimpleMod::hash(int n, int m){
  return n % m;
}

int HashLooseLoose::hash(int n, int m){
  int hash = 0;
  while (n != 0){
    hash += n % 10;
    n = n / 10;
  }
  hash = hash % m;
  return hash;
}

#include<mpi.h>
#include "relation.hpp"
#include<vector>
#include<map>

#include "hypercube.hpp"

using std::vector;
using std::array;


Hypercube::Hypercube(int m, int p){
  this->m = m;
  this->p = p;
  this->map = std::map<vector<int>, int>();
  int c = 0;
  for (int i =0; i < m; i++) {
    for (int j = 0; j < m; j++) {
      for (int k = 0; k < m; k++){
	map[vector<int> {i, j, k}] = c;
	c += 1;
      }
     }
  }
}


vector<int> Hypercube::get_machine(vector<int> entry, vector<string> vars){
  vector<int> res;
  if (vars[0] == "x"){
    for (int i=0; i < p; i++){
      res.push_back(this->map[vector<int> {entry[0] % p, entry[1] % p, i}]);
    } 
  } else if (vars[0] == "y") {
    for (int i=0; i < p; i++){
      res.push_back(this->map[vector<int> {i, entry[0] % p, entry[1] % p}]);
    } 
  } else {
    for (int i=0; i < p; i++){
      res.push_back(this->map[vector<int> {entry[1] % p, i, entry[0] % p}]);
    } 
  }
  return res;
}

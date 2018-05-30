#include<iostream>
#include<vector>
#include<string>
#include<map>

#include "relation.hpp"

using std::vector;
using std::string;


class Hypercube {
public:
  int m;
  int no_machines;
  std::map<vector<int>, int> map;
  
  Hypercube(int);
  vector<int> get_machine(int*, vector<string>);
  void send_buffer(int, int, vector<int>&, vector<int>&);
  Relation scatter_entries(int, vector<string>, Relation*);
  void gather_entries(int, int, int, Relation*, Relation*);
  Relation compute_triangles(Relation*);
};

#include<iostream>
#include<vector>
#include<string>
#include<map>

using std::vector;
using std::string;


class Hypercube {
public:
  int m;
  int p;
  std::map<vector<int>, int> map;
  
  Hypercube(int, int);

  vector<int> get_machine(vector<int>, vector<string>);
};

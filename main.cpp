#include<iostream>
#include "relation.hpp"
int main(){
  Relation<int, 2> r1 = Relation<int, 2>("facebook.dat");
  Relation<int, 3> res = Relation<int, 3>();
  vector<string> var1 {"x", "y"};
  vector<string> var2 {"y", "z"};

  join<int, 2, 2, 3, 1>(r1, var1, r1, var2, res);
  res.to_file("res");
}

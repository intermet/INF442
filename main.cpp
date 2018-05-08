#include<iostream>
#include "relation.hpp"
std::
int main(int argc, char** argv){
  Relation<int, 2> r1 = Relation<int, 2>(argv[1]);
  Relation<int, 3> r2 = Relation<int, 3>();
  Relation<int, 3> r3 = Relation<int, 3>();

  vector<string> var1 {"x", "y"};
  vector<string> var2 {"y", "z"};
  vector<string> var3 {"x", "z"};
  vector<string> var  {"x", "y", "z"};

  join<int, 2, 2, 3, 1>(r1, var1, r1, var2, r2);
  join<int, 3, 2, 3, 2>(r2, var, r1, var3, r3);
  r3.to_file("res");
}

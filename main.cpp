#include<iostream>
#include "relation.hpp"

int main(){
  Relation<int, 2> r = Relation<int, 2>("test");
  array<uint, 2> order {1,0};
  r.sort(order);
  r.to_file("ziyed.mustapha");
}

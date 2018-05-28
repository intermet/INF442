#include<iostream>
#include "relation.hpp"
#include<mpi.h>

#include<vector>



void send_buffer(int, int, int, vector<int*>&, vector<int*>&, int);
Relation distribued_join(Relation*, Relation*);
Relation ff(vector<Relation>&);
Relation multi_distribued_join(vector<Relation>&);


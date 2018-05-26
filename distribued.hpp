#include<iostream>
#include "relation.hpp"
#include<mpi.h>

#include<vector>



void send_buffer(int, int, int, vector<int*>&, vector<int*>&, int);
void gather_entries(vector<int*>&, Relation&);
Relation distribued_join(Relation*, Relation*);

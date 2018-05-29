#include<iostream>
#include<mpi.h>
#include "hash.hpp"
#include "relation.hpp"

#include<vector>

Relation distribued_join(Relation*, Relation*, Hasher h);
Relation ff(vector<Relation>&, Hasher h);
Relation multi_distribued_join(vector<Relation>&);


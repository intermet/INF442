#include<iostream>
#include "relation.hpp"
#include<mpi.h>
#include "hash.hpp"

#include<vector>

Relation distribued_join(Relation*, Relation*, Hasher h);
Relation ff(vector<Relation>&, Hasher h);


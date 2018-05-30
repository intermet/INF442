#include<iostream>
#include<mpi.h>
#include "hash.hpp"
#include "relation.hpp"

#include<vector>

Relation distribued_join(Relation*, Relation*, Hasher);
Relation task7_multi_join(vector<Relation>&, Hasher h);
Relation multi_distribued_join(vector<Relation>&, Hasher);


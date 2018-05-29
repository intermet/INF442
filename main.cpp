#include<iostream>
#include "distribued.hpp"
#include<mpi.h>

int main(int argc, char** argv){
  MPI_Init(&argc, &argv);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  Relation r1;
  Relation r2;
  Relation r3;

  vector<string> vars1 {"x", "y"};
  vector<string> vars2 {"y", "z"};
  vector<string> vars3 {"z", "u"};

  r1.set_vars(vars1);
  r2.set_vars(vars2);
  r3.set_vars(vars3);

  vector<Relation> r;
  Hasher h = HashSimpleMod();
  if (rank == 0){
    r1 = Relation("facebook.dat", vars1);
    r2 = Relation("facebook.dat", vars2);
    r3 = Relation("facebook.dat", vars3);
  }

  r = {r1, r2, r3};
  Relation res = ff(r, h);
  if (rank == 0){
    //res.to_file("res");
  }
  //distribued_join(&r1, &r2);
  MPI_Finalize();
  return 0;
}


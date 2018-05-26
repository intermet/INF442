#include<iostream>
#include "distribued.hpp"
#include<mpi.h>


int main(int argc, char** argv){
  MPI::Init(argc, argv);
  int rank = MPI::COMM_WORLD.Get_rank();
  vector<string> vars1 {"x", "x", "y"};
  vector<string> vars2 {"y", "z"};
  Relation r1 = Relation(vars1);
  Relation r2 = Relation(vars2);
  if (rank == 0){
    vector<string> v = join_vars(vars1, vars2);
    r1 = Relation(argv[1], 3);
    r2 = Relation(argv[2], 2);
    r1.to_file("r1");
    r2.to_file("r2");
    r1.set_vars(vars1);
    r2.set_vars(vars2);
    //Relation r = join(r1, r2);
    //r.to_file("joined");
  }
  Relation r = distribued_join(&r1, &r2);
  if (rank == 0){
    r.to_file("joined");
  }
  MPI::Finalize();
  return 0;
}


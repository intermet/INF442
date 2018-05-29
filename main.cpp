#include<iostream>
#include "distribued.hpp"
#include "hypercube.hpp"
#include<mpi.h>

int main(int argc, char** argv){
  MPI_Init(&argc, &argv);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  Hypercube h (3);

  Relation r1;
  if (rank == 0){
    r1 = Relation("facebook.dat", vector<string> {"x", "y"});
    h.compute_triangles(&r1);
  }
  // Relation r1;
  // Relation r2;
  // Relation r3;

  // vector<string> vars1 {"x", "y"};
  // vector<string> vars2 {"y", "z"};
  // vector<string> vars3 {"z", "u"};

  // r1.set_vars(vars1);
  // r2.set_vars(vars2);
  // r3.set_vars(vars3);

  // vector<Relation> r;
  // Hasher h = HashSimpleMod();
  // if (rank == 0){
  //   r1 = Relation("fb.dat", vars1);
  //   r2 = Relation("fb.dat", vars2);
  //   r3 = Relation("fb.dat", vars3);
  // }

  // r = {r1, r2, r3};
  // Relation res = ff(r, h);
  // if (rank == 0){
  //   std::cout << res.get_size() << "\n";
  //   //res.to_file("res");
  // }
  // //distribued_join(&r1, &r2);
  MPI_Finalize();
  return 0;
}


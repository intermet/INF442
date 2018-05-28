#include<iostream>
#include "distribued.hpp"
#include "hypercube.hpp"
#include<mpi.h>

//dev

int main(int argc, char** argv){
  MPI::Init(argc, argv);
  int rank = MPI::COMM_WORLD.Get_rank();
  vector<string> vars1 {"x", "y"};
  vector<string> vars2 {"y", "z"};
  vector<string> vars3 {"z", "u"};
  Relation r1 = Relation(vars1);
  Relation r2 = Relation(vars2);
  Relation r3 = Relation(vars3);
  if (rank == 0){
    r1 = Relation(argv[1], 2);
    r2 = Relation(argv[2], 2);
    r3 = Relation(argv[1], 2);
    r1.set_vars(vars1);
    r2.set_vars(vars2);
    r3.set_vars(vars3);
    //Relation r = join(r1, r2);
    //r.to_file("joined");
  }
  // Relation r = distribued_join(&r1, &r2);
  vector<Relation> r {r1, r2, r3};
  Relation joined = multi_distribued_join(r); 
  if (rank == 0){
    joined.to_file("joined");
  }
  MPI::Finalize();
  return 0;
}


// int main(int argc, char** argv){
//   vector<string> vars1 {"x", "y"};
//   vector<string> vars2 {"y", "z"};
//   Relation r1 = Relation(vars1);
//   Relation r2 = Relation(vars2);
//   r1 = Relation(argv[1], 2);
//   r2 = Relation(argv[2], 2);
//   r1.set_vars(vars1);
//   r2.set_vars(vars2);
//   Relation r = join(r1, r2);
//   std::cout << r.get_size() << "\n";
// }

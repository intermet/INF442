#include<iostream>
#include "distribued.hpp"
#include "hypercube.hpp"
#include<mpi.h>


void test_simple_join(int rank, char **argv){
  Relation r1;
  Relation r2;

  vector<string> vars1 {"x", "y"};
  vector<string> vars2 {"y", "z"};

  r1.set_vars(vars1);
  r2.set_vars(vars2);

  Hasher h = HashSimpleMod();
  if (rank == 0){
    r1 = Relation(argv[1], vars1);
    r2 = Relation(argv[1], vars2);
  }

  Relation res = distribued_join(&r1, &r2, h);

  if (rank == 0){
    res.to_file("res");
  }
  
}

void test_simple_task7_join(int rank, char **argv){
  Relation r1;
  Relation r2;

  vector<string> vars1 {"x", "y"};
  vector<string> vars2 {"y", "z"};

  r1.set_vars(vars1);
  r2.set_vars(vars2);

  Hasher h = HashSimpleMod();
  if (rank == 0){
    r1 = Relation(argv[1], vars1);
    r2 = Relation(argv[1], vars2);
  }

  vector<Relation> r {r1, r2};
  Relation res = ff(r, h);

  if (rank == 0){
    // res.to_file("res");
  }
  
}

void test_triangle_join(int rank, char **argv){
  Relation r1;
  Relation r2;
  Relation r3;

  vector<string> vars1 {"x", "y"};
  vector<string> vars2 {"y", "z"};
  vector<string> vars3 {"z", "x"};

  r1.set_vars(vars1);
  r2.set_vars(vars2);
  r3.set_vars(vars3);

  Hasher h = HashSimpleMod();
  if (rank == 0){
    r1 = Relation(argv[1], vars1);
    r2 = Relation(argv[1], vars2);
    r3 = Relation(argv[1], vars3);
  }

  Relation res = distribued_join(&r1, &r2, h);
  res.set_vars({"x", "y", "z"});
  res = distribued_join(&res, &r3, h);
  if (rank == 0){
     //res.to_file("res");
  }
}

void test_triangle_task7_join(int rank, char **argv){
  Relation r1;
  Relation r2;
  Relation r3;

  vector<string> vars1 {"x", "y"};
  vector<string> vars2 {"y", "z"};
  vector<string> vars3 {"z", "x"};

  r1.set_vars(vars1);
  r2.set_vars(vars2);
  r3.set_vars(vars3);

  Hasher h = HashSimpleMod();
  if (rank == 0){
    r1 = Relation(argv[1], vars1);
    r2 = Relation(argv[1], vars2);
    r3 = Relation(argv[1], vars3);
  }

  vector<Relation> r {r1, r2};
  Relation res = ff(r, h);
  res.set_vars({"x", "y", "z"});
  r = {res, r3};
  res = ff(r, h);
  if (rank == 0){
    res.to_file("res");
  }
}


void test_hypercube(int rank, char**argv){
  Hypercube h (2);
  Relation r1;
  if (rank == 0){
    r1 = Relation(argv[1], vector<string> {"x", "y"});
  }
  Relation res = h.compute_triangles(&r1);
  if (rank == 0){
    res.to_file("res");
  }
}

int main(int argc, char** argv){
  MPI_Init(&argc, &argv);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  //test_simple_join(rank, argv);
  // test_simple_task7_join(rank, argv);
  
  //test_triangle_join(rank, argv);
  // test_triangle_task7_join(rank, argv);

  test_hypercube(rank, argv);
  
  MPI_Finalize();
  return 0;
}



#include<vector>
#include<map>
#include<mpi.h>
#include "hypercube.hpp"

using std::vector;
using std::array;

void Hypercube::send_buffer(int receiver, int rank, vector<int> &to_send, vector<int> &to_recv){
  int size_recv;
  if (rank == 0){
    int size_recv = to_send.size();
    MPI_Send(&size_recv, 1, MPI_INT, receiver, 0, MPI_COMM_WORLD);
    MPI_Send(&to_send[0], size_recv, MPI_INT, receiver, 0, MPI_COMM_WORLD);
  } else if (rank == receiver){
    MPI_Recv(&size_recv, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    to_recv.resize(size_recv); 
    MPI_Recv(&to_recv[0], size_recv, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
}

Relation Hypercube::scatter_entries(int rank, vector<string> vars, Relation *r){
  vector<vector<int>> buffers;
  Relation res;
  buffers.resize(this->no_machines);
  if (rank == 0){
    int * entry;
    int size = r->get_size();
    for (int i = 0; i < size; i += 2) {
      entry = r->entry(i);
      for (int buf : this->get_machine(entry, vars)){
	buffers[buf].push_back(entry[0]);
	buffers[buf].push_back(entry[1]);
      }
    }
  }
  for (int i = 0; i < this->no_machines; i++){
    if (i != 0){
      send_buffer(i, rank, buffers[i], buffers[i]);
    }
  }
  res = Relation(buffers[rank], vars);
  return res;
}

Hypercube::Hypercube(int m){
  this->m = m;
  this->map = std::map<vector<int>, int>();
  int c = 0;
  for (int i =0; i < m; i++) {
    for (int j = 0; j < m; j++) {
      for (int k = 0; k < m; k++){
	map[vector<int> {i, j, k}] = c;
	c += 1;
      }
     }
  }
  this->no_machines = m * m * m;
}

vector<int> Hypercube::get_machine(int *entry, vector<string> vars){
  vector<int> res;
  if (vars[0] == "x"){
    for (int i=0; i < m; i++){
      res.push_back(this->map[vector<int> {entry[0] % m, entry[1] % m, i}]);
    } 
  } else if (vars[0] == "y") {
    for (int i=0; i < m; i++){
      res.push_back(this->map[vector<int> {i, entry[0] % m, entry[1] % m}]);
    } 
  } else {
    for (int i=0; i < m; i++){
      res.push_back(this->map[vector<int> {entry[1] % m, i, entry[0] % m}]);
    } 
  }
  return res;
}


void Hypercube::gather_entries(int rank, int root, int p, Relation *r, Relation *res){
  vector<int> gather_buffer;
  int joined_size[p];
  int displs[p];
  int size_total = 0;
  vector<int> to_send;
  int size = r->get_size();
  MPI_Gather(&size, 1, MPI_INT, &joined_size[0], 1, MPI_INT, root, MPI_COMM_WORLD);
  displs[0] = 0;
  for (int i = 1; i < p; i++){
    displs[i] = displs[i-1] + joined_size[i-1];
  }
  
  if (rank == root){
    for (int x: joined_size){
      size_total += x;
    } 
    gather_buffer.resize(size_total);
  }
  MPI_Gatherv(r->entries, size, MPI_INT, &gather_buffer[0], joined_size, displs, MPI_INT, root, MPI_COMM_WORLD);
  
  if (rank == root) {
    *res = Relation(gather_buffer, r->get_vars());
  }
}


Relation Hypercube::compute_triangles(Relation *r){
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int p;
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  Relation local1;
  Relation local2;
  Relation local3;
  Relation local;
  Relation res;
  local1 = scatter_entries(rank, vector<string> {"x", "y"}, r);
  local2 = scatter_entries(rank, vector<string> {"y", "z"}, r);
  local3 = scatter_entries(rank, vector<string> {"z", "x"}, r);

  local = join(local1, local2);
  local = join(local, local3);
  gather_entries(rank, 0, p, &local, &res);

  return res;
}

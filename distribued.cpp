#include<mpi.h>
#include "relation.hpp"
#include<vector>
#include "hash.hpp"

using std::vector;

void send_buffer(int sender, int receiver, int rank, vector<int> &to_send, vector<int> &to_recv){
  int size_recv;
  if (rank == sender){
    int size_recv = to_send.size();
    MPI_Send(&size_recv, 1, MPI_INT, receiver, sender, MPI_COMM_WORLD);
    MPI_Send(&to_send[0], size_recv, MPI_INT, receiver, sender, MPI_COMM_WORLD);
  } else if (rank == receiver){
    MPI_Recv(&size_recv, 1, MPI_INT, sender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    to_recv.resize(size_recv); 
    MPI_Recv(&to_recv[0], size_recv, MPI_INT, sender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
}


void scatter_entries(int rank, int root, int p, int var, Relation *r1, Relation *local, Hasher h){
  vector<vector<int>> buffers;
  int arity = r1->get_arity();
  int size = r1->get_size();
  int * entry;
  buffers.resize(p, vector<int>());
  if (rank == root){
    for (int i = 0; i < size; i += arity){
      entry = r1->entry(i);
      for (int j = 0; j < arity; j++){
      	buffers[h.hash(entry[var], p)].push_back(entry[j]);
      }
    }
  }
  for(int i = 0; i < p; i ++){
    if (i != root){
      send_buffer(root, i, rank, buffers[i], buffers[i]);
    }
  }
  *local = Relation(buffers[rank], r1->get_vars());
}


void gather_entries(int rank, int root, int p, Relation *r, Relation *res){
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


void g(Relation *r1, int idx, int p, vector<Relation> &res, Hasher h){
  res.resize(p, Relation());
  int arity = r1->get_arity();
  int *  entry;
  vector<vector<int>> buffers;
  buffers.resize(p);
  for (int i = 0; i < r1->get_size(); i += arity) {
    entry = r1->entry(i);
    for (int j = 0; j < arity; j++) {
      buffers[h.hash(entry[idx], p)].push_back(entry[j]);
    }
  }
  for (int  i = 0; i < p; i++) {
    res[i] = Relation(buffers[i], r1->get_vars());
  }
}


void f(Relation *r1, Relation *r2, Hasher h){
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int p;
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  vector<string> vars1 = r1->get_vars();
  vector<string> vars2 = r2->get_vars();
  vector<int> o1;
  vector<int> o2;

  vector<string> common = common_vars(r1->get_vars(), r2->get_vars(), o1, o2);
  string var = common[0];

  int idx1_var = 0;
  int idx2_var = 0;

  for(string v: vars1){
    if (v == var){
      break;
    }
    idx1_var += 1;
  }

  for(string v: vars2){
    if (v == var){
      break;
    }
    idx2_var += 1;
  }


  Relation local_r1;
  Relation local_r2;
  scatter_entries(rank, 0, p, idx2_var, r2, &local_r2, h);

  vector<Relation> r;
  g(r1, idx1_var, p, r, h);
  for(int i=0; i < p; i++){
    gather_entries(rank, i, p, &r[i], &local_r1);
  }
  *r1 = join(local_r1, local_r2);
}


Relation ff(vector<Relation> &r, Hasher h){
  int no_r = r.size();
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int p;
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  
  Relation res;

  Relation r1 = r[0];
  Relation r2 = r[1];


  vector<string> vars1 = r1.get_vars();
  vector<string> vars2 = r2.get_vars();
  vector<int> o1;
  vector<int> o2;

  vector<string> common = common_vars(r1.get_vars(), r2.get_vars(), o1, o2);
  string var = common[0];

  int idx1_var = 0;

  for(string v: vars1){
    if (v == var){
      break;
    }
    idx1_var += 1;
  }
  Relation local_r1;
  scatter_entries(rank, 0, p, idx1_var, &r1, &local_r1, h);
  for (int i = 1; i < no_r; i++){
    f(&local_r1, &r[i], h);
  } 
  
  gather_entries(rank, 0, p, &local_r1, &res);
  return res;
}

Relation distribued_join(Relation *r1, Relation *r2, Hasher h){
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int p;
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  vector<string> vars1 = r1->get_vars();
  vector<string> vars2 = r2->get_vars();
  vector<int> o1;
  vector<int> o2;
  common_vars(r1->get_vars(), r2->get_vars(), o1, o2);
  Relation res;
  vector<string> common = common_vars(r1->get_vars(), r2->get_vars(), o1, o2);
  string var = common[0];

  int idx1_var = 0;
  for(string v: vars1){
    if (v == var){
      break;
    }
    idx1_var += 1;
  }

  int idx2_var = 0;
  for(string v: vars2){
    if (v == var){
      break;
    }
    idx2_var += 1;
  }
  

  Relation local_r1;
  Relation local_r2;
  scatter_entries(rank, 0, p, idx1_var, r1, &local_r1, h);
  scatter_entries(rank, 0, p, idx2_var, r2, &local_r2, h);
  Relation local = join(local_r1, local_r2);
  gather_entries(rank, 0, p, &local, &res);
  
  return res;
}



Relation multi_distribued_join(vector<Relation> &relations){
  Relation res = relations[0];
  for (Relation r: relations){
    res = join(res, r);
  }
  return res;
}

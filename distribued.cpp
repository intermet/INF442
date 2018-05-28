#include<mpi.h>
#include "relation.hpp"
#include<vector>


using std::vector;
using std::array;


void send_buffer(int sender, int receiver, int rank, vector<int*> &to_send, vector<int*> &to_recv, int arity){
  int nb_entries;
  int buffer_size;
  if (rank == sender){
    nb_entries = to_send.size();
    buffer_size = nb_entries * arity;
    vector<int> send_buffer;
    for (int* p : to_send){
      for (int j = 0; j < arity; j++){
	send_buffer.push_back(p[j]);
      }
    }
    MPI_Send(&buffer_size, 1, MPI_INT, receiver, sender, MPI_COMM_WORLD);
    MPI_Send(&send_buffer[0], buffer_size, MPI_INT, receiver, sender, MPI_COMM_WORLD);
  } else if (rank == receiver){
    vector<int> recv_buffer;
    MPI_Recv(&buffer_size, 1, MPI_INT, sender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    recv_buffer.resize(buffer_size);
    MPI_Recv(&recv_buffer[0], buffer_size, MPI_INT, sender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    int *p;
    for (int i = 0; i < buffer_size/arity; i++){
      p = new int[arity];
      for (int j = 0; j < arity; j++) {
	p[j] = recv_buffer[arity * i + j];
      }
      to_recv.push_back(p);
    }
  }
}

void scatter_entries(int rank, int root, int p, int var, Relation *r1, Relation *local){
  vector<vector<int*>> buffers;
  vector<int*> buffer;
  int arity = r1->get_arity();
  buffers.resize(p, vector<int *>());
  int size = r1->get_size();

  if (rank == root) {
    for(int i=0; i < size; i++){
      int *entry = r1->entry(i);
      if (r1->satisfy_atom(entry)){
	buffers[entry[var] % p].push_back(entry);
      }
    } 
  }

  for (int i = 0; i < p; i++){
    if (i != root) {
      send_buffer(root, i, rank, buffers[i], buffer, arity);
    } else {
      buffer.resize(buffers[root].size(), new int[arity]);
      std::copy(buffers[root].begin(), buffers[root].end(), buffer.begin());
    }
  }

  *local = Relation(buffer, r1->get_arity());
  local->set_vars(r1->get_vars());
}


void gather_entries(int rank, int root, int p, Relation *r, Relation *res){
  vector<int> gather_buffer;
  int arity = r->get_arity();
  int joined_size[p];
  int displs[p];
  int size_total = 0;
  vector<int> to_send;
  for (int *p : r->entries){
    for (int i=0; i<arity; i++){
      to_send.push_back(p[i]);
    }
  }
  int size = to_send.size();
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
  MPI_Gatherv(&to_send[0], size, MPI_INT, &gather_buffer[0], joined_size, displs, MPI_INT, root, MPI_COMM_WORLD);
  
  if (rank == root) {
    vector<int*> entries;
    int arity = r->get_arity();
    int size = gather_buffer.size() / arity; 
    for (int i = 0; i < size; i++){
      int *p = new int[arity];
      for (int j = 0; j < arity; j++){
	p[j] = gather_buffer[i * arity + j]; 
      }
      entries.push_back(p);
    }
    *res = Relation(entries, arity);
    res->set_vars(r->get_vars());
  }
}


void g(Relation *r1, int idx, int p, vector<Relation> &res){
  res.resize(p, Relation(r1->get_arity()));
  for (int* e: r1->entries){
    res[e[idx] % p].add(e);
  }
}


void f(Relation *r1, Relation *r2){
  int rank = MPI::COMM_WORLD.Get_rank();
  int p = MPI::COMM_WORLD.Get_size();

  int arity1 = r1->get_arity();
  int arity2 = r1->get_arity();

  vector<string> vars1 = r1->get_vars();
  vector<string> vars2 = r2->get_vars();

  vector<string> common = common_vars(r1->get_vars(), r2->get_vars());
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
  scatter_entries(rank, 0, p, idx2_var, r2, &local_r2);

  vector<Relation> r;
  local_r1.set_vars(r1->get_vars());
  g(r1, idx1_var, p, r);
  for(int i=0; i < p; i++){
    gather_entries(rank, i, p, &r[i], &local_r1);
  }
  local_r1.set_vars(r1->get_vars());
  
  local_r1.to_file("data/r1" + std::to_string(rank));
  local_r2.to_file("data/r2" + std::to_string(rank));
  *r1 = join(local_r1, local_r2);
}

Relation ff(vector<Relation> &r){
  int no_r = r.size();
  int rank = MPI::COMM_WORLD.Get_rank();
  int p = MPI::COMM_WORLD.Get_size();

  Relation r1 = r[0];
  Relation r2 = r[1];

  vector<string> vars1 = r1.get_vars();
  vector<string> vars2 = r2.get_vars();

  vector<string> common = common_vars(r1.get_vars(), r2.get_vars());
  string var = common[0];

  int idx1_var = 0;

  for(string v: vars1){
    if (v == var){
      break;
    }
    idx1_var += 1;
  }
  Relation local_r1;
  scatter_entries(rank, 0, p, idx1_var, &r1, &local_r1);


  for (int i = 1; i < no_r; i++){
    f(&local_r1, &r[i]);
  } 
  Relation res;
  gather_entries(rank, 0, p, &local_r1, &res);
  if (rank == 0) {
  }
  return res;
}


Relation distribued_join(Relation *r1, Relation *r2){
  int rank = MPI::COMM_WORLD.Get_rank();
  int p = MPI::COMM_WORLD.Get_size();

  int arity1 = r1->get_arity();
  int arity2 = r2->get_arity();

  vector<string> vars1 = r1->get_vars();
  vector<string> vars2 = r2->get_vars();

  vector<string> common = common_vars(r1->get_vars(), r2->get_vars());
  string var = common[0];

  int idx1_var = 0;
  int idx2_var = 0;

  vector<int*> buffer_r1;
  vector<int*> buffer_r2;

  Relation res;
  
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
  scatter_entries(rank, 0, p, idx1_var, r1, &local_r1);
  scatter_entries(rank, 0, p, idx2_var, r2, &local_r2);

  Relation join_r1r2 = join(local_r1, local_r2);
  
  res.set_vars(join_vars(vars1, vars2));
  gather_entries(rank, 0, p, &join_r1r2, &res);

  f(&join_r1r2, r2);
  return res;
}




Relation multi_distribued_join(vector<Relation> &relations){
  Relation res = relations[0];
  for (Relation r: relations){
    res = join(res, r);
  }
  return res;
}

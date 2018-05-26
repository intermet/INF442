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

void scatter_entries(int rank, int p, int var, vector<string> vars, Relation *r1, vector<int*> &buffer){
  vector<vector<int*>> buffers;
  int arity = r1->get_arity();
  buffers.resize(p, vector<int *>());
  int size = r1->get_size();

  if (rank == 0) {
    for(int i=0; i < size; i++){
      int *entry = r1->entry(i);
      if (r1->satisfy_atom(entry)){
	buffers[entry[var] % p].push_back(entry);
      }
    } 

  }

  for (int i = 1; i < p; i++){
    send_buffer(0, i, rank, buffers[i], buffer, arity);
  }

  if (rank == 0){
    buffer.resize(buffers[0].size(), new int[arity]);
    std::copy(buffers[0].begin(), buffers[0].end(), buffer.begin());
  }
}


void gather_entries(int rank, int p, vector<int> &gather_buffer, Relation &r){
  int arity = r.get_arity();
  int joined_size[p];
  int displs[p];
  int size_total = 0;
  vector<int> to_send;
  for (int *p : r.entries){
    for (int i=0; i<arity; i++){
      to_send.push_back(p[i]);
    }
  }
  int size = to_send.size();
  MPI_Gather(&size, 1, MPI_INT, &joined_size[0], 1, MPI_INT, 0, MPI_COMM_WORLD);

  displs[0] = 0;
  for (int i = 1; i < p; i++){
    displs[i] = displs[i-1] + joined_size[i-1];
  }

  
  if (rank == 0){
    for (int x: joined_size){
      size_total += x;
    } 
    gather_buffer.resize(size_total);
  }
  MPI_Gatherv(&to_send[0], size, MPI_INT, &gather_buffer[0], joined_size, displs, MPI_INT, 0, MPI_COMM_WORLD);
}


Relation distribued_join(Relation *r1, Relation *r2){
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
  scatter_entries(rank, p, idx1_var, vars1, r1, buffer_r1);
  //std::cout << "ID=" << rank << " " << buffer_r1.size()/2 << "\n";
  
  scatter_entries(rank, p, idx2_var, vars2, r2, buffer_r2);
  //std::cout << "ID=" << rank << " " << buffer_r2.size()/2 << "\n";


  Relation local_r1 = Relation(buffer_r1, arity1);
  Relation local_r2 = Relation(buffer_r2, arity2);
  local_r1.set_vars(vars1);
  local_r2.set_vars(vars2);

  local_r1.to_file("data/r1_"+ std::to_string(rank));
  local_r2.to_file("data/r2_"+ std::to_string(rank));
  /*
  std::cout << local_r1.get_size() << "\n";
  std::cout << local_r2.get_size() << "\n";
  Relation join_r1r2 = join(local_r1, local_r2);
  */

  Relation join_r1r2 = join(local_r1, local_r2);
  join_r1r2.to_file("data/joined" + std::to_string(rank));

  vector<int> gather_buffer;
  gather_entries(rank, p, gather_buffer, join_r1r2);

  vector<int*> entries;
  if (rank == 0){
    int arity = join_r1r2.get_arity();
    int size = gather_buffer.size() / arity; 
    for (int i = 0; i < size; i++){
      int *p = new int[arity];
      for (int j = 0; j < arity; j++){
	p[j] = gather_buffer[i * arity + j]; 
      }
      entries.push_back(p);
    }
    res = Relation(entries, arity);
  }
  
  return res;
}

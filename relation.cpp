#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<array>
#include<string>
#include<algorithm>
#include<map>
#include<set>

#include "relation.hpp"

using std::vector;
using std::string;
using std::array;



int compare(int* x, int* y, vector<int> order) {
  for (int i: order){
    if (x[i] < y[i]){
      return 1;
    } else if (x[i] > y[i]){
      return -1;
      }
  }
  return 0;
}


int compare2(int* x, int* y, vector<int> order1, vector<int> order2) {
  int size = order1.size();
  for (int i = 0; i < size; i++){
    if (x[order1[i]] < y[order2[i]]){
      return 1;
    } else if (x[order1[i]] > y[order2[i]]){
      return -1;
      }
  }
  return 0;
}


vector<string> common_vars(vector<string> vars1, vector<string> vars2, vector<int> &o1, vector<int> &o2){
  std::map<string, int> map;
  vector<string> res;
  int size1 = vars1.size();
  int size2 = vars2.size();
  for (int i = 0; i < size1; i++){
    if (map.count(vars1[i]) == 0){
      map[vars1[i]] = i;
    }
  }
  for (int i = 0; i < size2; i++){
    if (map.count(vars2[i]) == 1){
      res.push_back(vars2[i]);
      o2.push_back(i);
      o1.push_back(map[vars2[i]]);
    }
  }
  return res;
}


vector<string> join_vars(vector<string> vars1, vector<string> vars2){
  std::set<string> map;
  vector<string> res;
  for (string var: vars1){
    if (map.count(var) == 0){
      map.insert(var);
      res.push_back(var);
    }
  }
  for (string var: vars2){
    if (map.count(var) == 0){
      map.insert(var);
      res.push_back(var);
    }
  }
  return res;
}
  
vector<int> Relation::find_order(vector<string> sub_var){
  std::set<string> map;
  for (string v: sub_var){
    map.insert(v);
  }
  int c = 0;
  int arity = vars.size();
  int d = vars.size() -1;
  vector<int> res;
  res.resize(arity);
  for (int i=0; i < arity; i++){
    if(map.count(vars[i]) == 1){
      res[i] = c;
      c += 1;
    } else {
      res[i] = d;
      d -= 1;
    }
  }

  return res;
}

Relation::Relation(){
  
};

Relation::Relation(vector<int> e, vector<string> vars){
  this->set_vars(vars);
  this->size = e.size();
  this->entries = new int[this->size];
  for (int i = 0; i < this->size; i++){
    this->entries[i] = e[i];
  }
}

Relation::Relation(string fname, vector<string> vars){
  std::ifstream file (fname);
  vector<int> e;

  this->set_vars(vars);

  string line;
  int x;
  while (std::getline(file, line)){
    std::istringstream ss(line);
    for (int i = 0; i < this->arity; ++i){
      ss >> x;
      e.push_back(x);
    }
  }
  file.close();

  this->size = e.size();
  this->entries = new int[this->size];
  for (int i = 0; i < this->size; i++){
    this->entries[i] = e[i];
  }
};


int* Relation::entry(int i){
  return &(this->entries[i]);
}

vector<string> Relation::get_vars(){
  return this->vars;
}

int Relation::get_size(){
  return this->size;
}

int Relation::get_arity(){
  return this->arity;
}

void Relation::set_vars(vector<string> vars){
  this->vars = vars;
  this->arity = vars.size();
}


void Relation::to_file(string name){
  std::ofstream file;
  file.open(name);
  for (int i = 0; i < this->size; i++){
    if ((i + 1) % this->arity != 0) {
      file << entries[i] << " ";
    } else {
      file << entries[i] << "\n";
    }
  }
  file.close();
}

void Relation::sort(vector<int> order) {
  vector<int*> buff;
  int j = 0;
  int *entry = new int[this->arity];
  for (int i = 0; i < this->size; i++){
    entry[j] = this->entries[i];
    j += 1;
    if (j % this->arity == 0){
      buff.push_back(entry);
      entry = new int[this->arity];
      j = 0;
    }
  }
  
  std::sort(buff.begin(), buff.end(),  [order](int* x, int* y) -> bool
  					 {
  					   return (0 < compare(x, y, order));
  					 });
  int i = 0;
  for (int* x: buff) {
    for (int j = 0; j < this->arity; j++) {
      this->entries[i] = x[j]; 
      i += 1;
    }
  }
}


bool Relation::satisfy_atom(int entry){
  std::map<string, int> map;
  string x;
  for (int i = 0; i < this->arity; ++i) {
    x = this->vars[i];
    if (map.count(x) == 0){
      map[x] = this->entries[entry + i];
    } else {
      if (map[x] != this->entries[entry + i]){
	return false;
      }
    }
  }
  return true;
}

void join_add_entry(int* t1, int* t2, vector<int> idx1, vector<int> idx2, vector<int> &buff){
  for (int x: idx1){
    buff.push_back(t1[x]);
  }
  
  for (int x: idx2){
    buff.push_back(t2[x]);
  }
}


void get_index(vector<string> vars1, vector<string> vars2, vector<int> &res1, vector<int> &res2){
  std::set<string> map;
  int arity1 = vars1.size();
  int arity2 = vars2.size();
  for (int k = 0; k < arity1; k++){
    if(map.count(vars1[k]) == 0){
      res1.push_back(k);
      map.insert(vars1[k]);
    }
  }

  for (int k = 0; k < arity2; k++){
    if(map.count(vars2[k]) == 0){
      res2.push_back(k);
      map.insert(vars2[k]);
    }
  }
}

Relation join(Relation &r1, Relation &r2){
  vector<string> vars1 = r1.get_vars();
  vector<string> vars2 = r2.get_vars();
  vector<string> res_vars = join_vars(vars1, vars2);
  vector<int> o1;
  vector<int> o2;
  vector<string> common = common_vars(vars1, vars2, o1, o2);

  vector<int> vars1_idx;
  vector<int> vars2_idx;
  get_index(vars1, vars2, vars1_idx, vars2_idx);

  vector<int> order1 = r1.find_order(common);
  vector<int> order2 = r2.find_order(common);


  vector<int> common_order;
  int common_order_size = common.size();
  for (int i = 0; i < common_order_size; i++){
    common_order.push_back(i);
  }

  Relation res;

  vector<int> buff;
  
  r1.sort(order1);
  r2.sort(order2);

  int size1 = r1.get_size();
  int size2 = r2.get_size();

  
  int arity1 = r1.get_arity();
  int arity2 = r2.get_arity();
  
  int i = 0;
  int j = 0;
  int k = 0;
  int c;

  int * t1;
  int * t2;

  int a = 0;


  while (i < size1 && j < size2){
    if (r1.satisfy_atom(i)){
      if (r2.satisfy_atom(j)){
	t1 = r1.entry(i);
	t2 = r2.entry(j);
	c =  compare2(t1, t2, o1, o2);
	if (c == 1){
	  k = 0;
	  i += arity1;
	} else if (c == -1){
	  k = 0;
	  j += arity2;
	} else {
	  if (k == 0){
	    k = j;
	    while (k < size2) {
	      t2 = r2.entry(k);
	      c = compare2(t1, t2, o1, o2);
	      if (c == 0){
		join_add_entry(t1, t2, vars1_idx, vars2_idx, buff);
		a += 1;
		k += arity2;
	      } else {
		break;
	      }
	    }
	} else {
	    for (int l = j; l < k; l += arity2){
	      t2 = r2.entry(l);
	      a += 1;
	      join_add_entry(t1, t2, vars1_idx, vars2_idx, buff);
	    }
	  }
	i += arity1;
	}
      } else {
	j += arity2;
      }
    } else {
      i += arity1;
    }
  }
  res = Relation(buff, res_vars);
  return res;
}



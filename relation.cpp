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


vector<string> common_vars(vector<string> vars1, vector<string> vars2){
  std::set<string> map;
  vector<string> res;
  for (string var: vars1){
    if (map.count(var) == 0){
      map.insert(var);
    }
  }
  for (string var: vars2){
    if (map.count(var) == 1){
      res.push_back(var);
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

Relation::Relation(int a){
  this->arity = a;
};


Relation::Relation(vector<int*> &ent, int a){
  this->entries = ent;
  this->arity = a; 
}

Relation::Relation(vector<string> v) {
  vars = v;
  arity = v.size();
}


int Relation::get_size(){
  return this->entries.size();
}

int Relation::get_arity(){
  return this->arity;
}


vector<string> Relation::get_vars(){
  return this->vars;
}

void Relation::set_vars(vector<string> vars){
  this->vars = vars;
  this->arity = vars.size();
}

int* Relation::entry(int i){
  return entries[i];
}

Relation::Relation(string fname, int a){
  std::ifstream file (fname);

  this->arity = a;
  this->entries = vector<int*> ();  
  
  string line;
  int *entry;
  while (std::getline(file, line)){
    entry = new int[a]; 
    std::istringstream ss(line);
    for (int i = 0; i < a; ++i){
      ss >> entry[i];
    }
    this->entries.push_back(entry);
  }
  
  file.close();
};

void Relation::add(int* e){
  entries.push_back(e);
}


void Relation::to_file(string name){
  std::ofstream file;
  file.open(name);
  int *entry;
  for (int i = 0; i < get_size(); i++){
    entry = this->entry(i);
    for (int i=0; i<arity-1; i++){
      file << entry[i] << " ";
    }
    file << entry[arity - 1] << std::endl;
  }
  file.close();
}

void Relation::sort(vector<int> order){
  std::sort(this->entries.begin(), this->entries.end(), [order](int* x, int* y)
	    {
	      return (0 < compare(x, y, order));
	    });
}


bool Relation::satisfy_atom(int* entry){
  std::map<string, int> map;
  string x;
  int arity = this->vars.size();
  for (int i=0; i < arity; ++i) {
    x = vars[i];
    if (map.count(x) == 0){
      map[x] = entry[i];
    } else {
      if (map[x] != entry[i]){
	return false;
      }
    }
  }
  return true;
}

int* Relation::project_entry(int* entry, vector<string> subvars){
  std::set<string> map;
  int j = 0;
  for (string x: subvars){
    map.insert(x);
  }

  int res_size = subvars.size();
  int* res = new int[res_size];
  
  for (int i = 0; i < arity; i++){
    if (map.count(vars[i]) == 1){
      res[j] = entry[i];
      j += 1;
    }
  }
  return res;
}

void Relation::project(vector<string> subvars, Relation &res){
  vector<int> pentry;
  vector<vector<int>> ent;
  for(int* entry: entries){
    if (satisfy_atom(entry)){
      res.add(project_entry(entry, subvars));
    }
  }
}


void join_add_entry(vector<string> vars1, int* t1, int arity1, vector<string> vars2, int* t2, int arity2, Relation &r){
  std::set<string> map;
  int* entry = new int[r.get_arity()];
  map.clear();
  int l = 0;
  for(int k=0; k < arity1; k++){
    if(map.count(vars1[k]) == 0){
      entry[l] = t1[k];
      map.insert(vars1[k]);
      l += 1;
    }
  }
  for(int k=0; k < arity2; k++){
    if(map.count(vars2[k]) == 0){
      entry[l] = t2[k];
      map.insert(vars2[k]);
      l += 1;
    }
  }
  r.add(entry);
}

Relation join(Relation &r1, Relation &r2){
  vector<string> vars1 = r1.get_vars();
  vector<string> vars2 = r2.get_vars();
  vector<string> res_vars = join_vars(vars1, vars2);
  vector<string> common = common_vars(vars1, vars2);


  vector<int> order1 = r1.find_order(common);
  vector<int> order2 = r2.find_order(common);

  vector<int> common_order;
  int common_order_size = common.size();
  for (int i = 0; i < common_order_size; i++){
    common_order.push_back(i);
  }

  Relation res(res_vars);
  
  r1.sort(order1);
  r2.sort(order2);
  
  int i = 0;
  int j = 0;
  int k = 0;
  
  int size1 = r1.get_size();
  int size2 = r2.get_size();
  
  int c;
    
  int* t1 = new int[r1.get_arity()];
  int* t2 = new int[r2.get_arity()];


  int* pt1 = new int[r1.get_arity()];
  int* pt2 = new int[r2.get_arity()];

  r1.to_file("sorted1");
  r2.to_file("sorted2");

  while (i < size1 && j < size2) {
    std::cout << i << "\n";
    t1 = r1.entry(i);
    t2 = r2.entry(j);

    if (r1.satisfy_atom(t1)) {
      if (r2.satisfy_atom(t2)) {
	pt1 = r1.project_entry(t1, common);
	pt2 = r2.project_entry(t2, common);
	c = compare(pt1, pt2, common_order);
	if (c == 1){
	  i += 1;
	} else if (c == -1){
	  j += 1;
	} else {
	  k = j;
	  while (true && k < size2){
	    t2 = r2.entry(k);
	    pt2 = r2.project_entry(t2, common);
	    c = compare(pt1, pt2, common_order);
	    if (c == 0){
	      join_add_entry(vars1, t1, r1.get_arity(),vars2, t2, r2.get_arity(), res);
	      k += 1;
	    } else {
	      break;
	    }
	  }
	  i += 1;
	}
      } else {
	j += 1;
      }
    } else {
      i += 1;
    }
  }
  
  // while (i < size1  && j < size2){
  //   t1 = r1.entry(i);
  //   t2 = r2.entry(j);
  //   if (r1.satisfy_atom(t1)) {
  //     if (r2.satisfy_atom(t2)){
  // 	pt1 = r1.project_entry(t1, common);
  // 	pt2 = r2.project_entry(t2, common);
  // 	c = compare(pt1, pt2, common_order);

  // 	if (c == 0){
  // 	  join_add_entry(vars1, t1, r1.get_arity(),vars2, t2, r2.get_arity(), res);
  // 	  j += 1;
  // 	} else if (c == 1){
  // 	  i += 1;
  // 	} else {
  // 	  j += 1;
  // 	}
  //     } else {
  // 	j += 1;
  //     }
  //   } else {
  //     i += 1;
  //   }
  // }
  return res;
}



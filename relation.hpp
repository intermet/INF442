#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<array>
#include<string>
#include<algorithm>
#include<map>
#include<set>

using std::vector;
using std::array;
using std::string;

vector<string> join_var_tuple(vector<string> var1, vector<string> var2){
  std::set<string> map;
  vector<string> res;
  for (string var: var1){
    if (map.count(var) == 0){
      map.insert(var);
      res.push_back(var);
    }
  }
  for (string var: var2){
    if (map.count(var) == 0){
      map.insert(var);
      res.push_back(var);
    }
  }
  return res;
}

vector<string> common_var(vector<string> var1, vector<string> var2){
  std::set<string> map;
  vector<string> res;
  for (string var: var1){
    if (map.count(var) == 0){
      map.insert(var);
    }
  }
  for (string var: var2){
    if (map.count(var) == 1){
      res.push_back(var);
    }
  }
  return res;
  
}

template<typename T, uint arity>
int compare(array<uint, arity> order, array<T, arity> x, array<T, arity> y){
  for(uint i: order){
    if (x[i] < y[i]){
      return 1;
    } else if (x[i] > y[i]){
      return -1;
    }
  }
  return 0;
}



template<typename T, uint arity>
class Relation {
private:
  vector<array<T, arity>> entries;

public:

  Relation(){
    
  }


  Relation(string fname){
    std::ifstream file (fname);

    string line;

    array<T, arity> entry;
    
    while (std::getline(file, line)){
      std::istringstream ss(line);
      for (uint i=0; i < arity; ++i) {
	ss >> entry[i];
      }
      entries.push_back(entry);
    }

    file.close();
  }

  uint get_arity(){
    return arity;
  }

  array<T, arity> entry(uint i){
    return entries[i];
  }
  
  void add(array<T, arity> &entry) {
    entries.push_back(entry);
  };

  uint size() {
    return entries.size();
  }

  void to_file(string name){
    std::ofstream file;
    file.open(name);

    for (auto entry: entries){
      for (int i=0; i<arity-1; i++){
	  file << entry[i] << " ";
      }
      file << entry[arity - 1] << std::endl;
    }
  }


  array<uint, arity> find_order(vector<string> var, vector<string> sub_var){
    std::set<string> map;
    for (auto v: sub_var){
      map.insert(v);
    }
    uint c = 0;
    uint d = arity - 1;
    array<uint, arity> res;
    for (uint i=0; i<arity; i++){
      if(map.count(var[i]) == 1){
	res[i] = c;
	c += 1;
      } else {
	res[i] = d;
	d -= 1;
      }
    }
    return res;
  }

  bool satisfy_atom(array<T, arity> entry, vector<string> var){
    if (var.size() != arity){
      return false;
    }
    std::map<string, T> map;
    string x;
    for (auto i=0; i < arity; ++i) {
      x = var[i];
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

   
  bool sort(array<uint, arity> order){
    if (order.size() != arity){
      return false;
    }
    std::sort(entries.begin(), entries.end(), [this, order](array<T, arity> &x, array<T, arity> &y)
	      {
		return (0 <= compare<T, arity>(order, x, y));
		  });
    return true;
  }

  template<uint n>
  void project_entry(array<T, arity> entry, vector<string> var, vector<string> subvar, array<T, n> &res){
    std::set<string> map;
    uint j = 0;
    for (auto x: subvar){
      map.insert(x);
    }
    for (int i=0; i<entry.size(); i++){
      if (map.count(var[i]) == 1){
	res[j] = entry[i];
	j += 1;
      }
    }
  }

  template<uint arity_p>
  bool project(Relation<T, arity_p> &r, vector<string> var){
    uint expected_arity = join_var_tuple(var, var).size(); 
    if (arity_p != expected_arity){
      return false;
    }
    array<T, arity_p> pentry;
    vector<T> ventry;
    for(auto entry: entries){
      if (satisfy_atom(entry, var)){
	ventry = project_entry(entry, var);
	std::copy_n(ventry.begin(), arity_p, pentry.begin());
	r.add(pentry);
      }
    }
    return true;
  }
  
};


template<typename T, uint arity1, uint arity2, uint res_arity>
void join_add_entry(array<T, arity1> t1, vector<string> var1, array<T, arity2> t2,vector<string> var2, Relation<T, res_arity> &res){
  std::set<string> map;
  int l;
  array<T, res_arity> entry;
  
  map.clear();
  l = 0;
  for(uint k=0; k<arity1; k++){
    if(map.count(var1[k]) == 0){
      entry[l] = t1[k];
      map.insert(var1[k]);
      l += 1;
    }
  }
  for(uint k=0; k<arity2; k++){
    if(map.count(var2[k]) == 0){
      entry[l] = t2[k];
      map.insert(var2[k]);
      l += 1;
    }
  }
  res.add(entry);
}



template<typename T, uint arity1, uint arity2, uint res_arity, uint n>
bool join(Relation<T, arity1> &r1, vector<string> var1, Relation<T, arity2> &r2, vector<string> var2, Relation<T, res_arity> &res_r){

  vector<string> var_res = join_var_tuple(var1, var2);
  vector<string> common = common_var(var1, var2);
  array<uint, arity1> order1 = r1.find_order(var1, common);
  array<uint, arity2> order2 = r2.find_order(var2, common);
  array<uint, n> common_order ;
  for (uint i = 0; i < n; i++){
    common_order[i] = i;
  }

  r1.sort(order1);
  r2.sort(order2);

  uint i = 0;
  uint j = 0;

  int c;
  array<T, arity1> t1;
  array<T, arity2> t2;
  array<T, n> pt1;
  array<T, n> pt2;

  while (i < r1.size()  && j < r2.size()){
    t1 = r1.entry(i);
    t2 = r2.entry(j);
    if (r1.satisfy_atom(t1, var1)) {
	if (r2.satisfy_atom(t2, var2)){
	  r1.template project_entry<n>(t1, var1, common, pt1);
	  r2.template project_entry<n>(t2, var2, common, pt2);
	  
	  c = compare<T, n>(common_order, pt1, pt2);
	  if (c == 0){
	    join_add_entry<T, arity1, arity2, res_arity>(t1, var1, t2, var2, res_r);
	    j += 1;
	  } else if (c == 1){
	    i+= 1;
	  } else {
	    j += 1;
	  }
	} else {
	  j += 1;
	}
    } else {
      i += 1;
    }
  }
  return true; 
}

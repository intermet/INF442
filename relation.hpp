#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<array>
#include<string>
#include<algorithm>

using std::vector;
using std::array;
using std::string;


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

  bool compare(array<uint, arity> order, array<T, arity> x, array<T, arity> y){
    for(uint i: order){
      if (x[i] < y[i]){
	return true;
      } else if (x[i] > y[i]){
	return false;
      }
    }
    return true;
  }

  void sort(array<uint, arity> order){
    std::sort(entries.begin(), entries.end(), [this, order](array<T, arity> &x, array<T, arity> &y)
	      {
		return compare(order, x, y);
		  });
  }
  
};

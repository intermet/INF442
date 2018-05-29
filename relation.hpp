#ifndef RELATION_HPP
#define RELATION_HPP
#include<iostream>
#include<vector>
#include<string>

using std::vector;
using std::string;


vector<string> common_vars(vector<string>, vector<string>, vector<int>&, vector<int>&);
 
class Relation {
private:
  int size;
  int arity;
  vector<string> vars;
  
public:
  int* entries;
  Relation(void);
  Relation(vector<int>, vector<string>);
  Relation(string, vector<string>);
  
  int get_size(void);
  int get_arity(void);

  int* entry(int i);

  vector<string> get_vars(void);
  void set_vars(vector<string>);

  void to_file(string);
  bool satisfy_atom(int);
  
  vector<int> find_order(vector<string>);

  vector<int> project_entry(int, vector<string>);
  
  void project(vector<string>, Relation&);
  
  void sort(vector<int>);
};

void get_index(vector<string>, vector<string>, vector<int>&, vector<int>&);
Relation join(Relation&, Relation&);
vector<string> join_vars(vector<string>, vector<string>);

void f();
#endif

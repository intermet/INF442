#include<iostream>
#include<vector>
#include<string>

using std::vector;
using std::string;

class Relation {
private:
  int size;
  int arity;
  vector<string> vars;
  
public:
  vector<int*> entries;
  Relation(void);
  Relation(int);
  Relation(vector<int*>&, int);
  Relation(string, int);
  Relation(vector<string>);

  int get_size(void);
  int get_arity(void);

  int* entry(int i);

  vector<string> get_vars(void);
  void set_vars(vector<string>);

  void add(int*);
  
  void to_file(string);
  bool satisfy_atom(int*);
  vector<int> find_order(vector<string>);

  int* project_entry(int*, vector<string>);

  void project(vector<string>, Relation&);
  
  void sort(vector<int>);


};

Relation join(Relation&, Relation&);
vector<string> common_vars(vector<string>, vector<string>);
vector<string> join_vars(vector<string>, vector<string>);

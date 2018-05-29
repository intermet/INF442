class Hasher {
public:
  virtual int hash(int, int){
    return 0;
  };
};

class HashSimpleMod : public Hasher{
public:
  HashSimpleMod(){
  }

  int hash(int, int);
  
};

class HashLooseLoose : public Hasher{
public:
  HashLooseLoose(){
    
  }

  int hash(int, int);
  
};


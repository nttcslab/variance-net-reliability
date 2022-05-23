#ifndef EQOPT_BDDVARIANCE_HPP
#define EQOPT_BDDVARIANCE_HPP

#include "common.hpp"
#include "graph.hpp"
#include "bdd.hpp"

#include <vector>
#include <unordered_map>
#include <unordered_set>

class BDDVariance{
public:
  const BDD& B;
  size_t leng;
  size_t dim;
  std::vector<double> p;
  std::vector<double> q;
  std::vector<double> s;
  std::vector<double> eval;
  std::unordered_map<std::pair<addr_t, addr_t>, double, HashPaddr> vval;
  
  BDDVariance(const BDD& _B): B(_B) {init();};
          
  void init(){
    leng = B.size();
    dim = B.numvars();
  }
  
  double calculate(const std::vector<double>& _p, const std::vector<double>& _s);
  
  size_t hashBucketSize(){
    return vval.bucket_count();
  }
  size_t hashEntrySize(){
    return vval.size();
  }
  void setBucketSize(size_t bsize){
    vval.rehash(bsize);
  }
  
private:
  double calculateInner(addr_t u, addr_t v);
};

#endif // EQOPT_BDDVARIANCE_HPP

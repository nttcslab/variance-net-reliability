#ifndef EQOPT_BDD_HPP
#define EQOPT_BDD_HPP

#include "common.hpp"
#include "graph.hpp"

#include <vector>

class BDDNode{
public:
  int lv;
  addr_t lo;
  addr_t hi;
  
  BDDNode(int _lv, addr_t _lo, addr_t _hi): lv(_lv), lo(_lo), hi(_hi) {};
  BDDNode(): lv(0), lo(0), hi(0) {};
          
  bool operator==(const BDDNode& right) const {
    return lv == right.lv && lo == right.lo && hi == right.hi;
  }
  bool operator<(const BDDNode& right) const {
    return lv > right.lv;
  }
};

class BDD{
public:
  std::vector<BDDNode> nodes;
  std::vector<int> vars;
  std::vector<int> invvars;
  size_t dim;
  
  BDD() {};
  
  inline size_t size() const{
    return nodes.size();
  }
  inline size_t numvars() const{
    return vars.size();
  }
  inline int var(int i) const{
    return vars[i];
  }
  inline addr_t loid(addr_t i) const{
    return nodes[i].lo;
  }
  inline addr_t hiid(addr_t i) const{
    return nodes[i].hi;
  }
  inline int level(addr_t i) const{
    return nodes[i].lv;
  }
  
  bool readfromFile(const char *filename);
  int readOrderfromFile(const char *filename, const Graph& g);
  double expectProb(const std::vector<double>& p) const;
  
  void dump() const{
    for(const auto& nd: nodes){
      printf("(%d,%lld,%lld)\n", nd.lv, nd.lo, nd.hi);
    }
  }
};

#endif // EQOPT_ZDD_HPP

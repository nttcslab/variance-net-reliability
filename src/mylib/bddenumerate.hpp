#ifndef VARIANCE_BDDENUMERATE_HPP
#define VARIANCE_BDDENUMERATE_HPP

#include "common.hpp"
#include "graph.hpp"
#include "bdd.hpp"

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <bitset>

template <size_t N>
class BDDEnumerate{
public:
  const BDD& B;
  size_t leng;
  size_t dim;
private:
  std::vector<int> skiplist;
  std::bitset<N> base;
  
public:
  
  BDDEnumerate(const BDD& _B): B(_B) {init();};
          
  void init(){
    leng = B.size();
    dim = B.numvars();
  }
  
  void calculate(std::vector<std::bitset<N>>& res){
    res.clear();
    skiplist.reserve(dim);
    addr_t rID = leng-1;
    
    for(int i=0; i<B.level(rID); ++i){
      skiplist.emplace_back(i);
    }
    calculateInner(rID, res);
  }
  
private:
  void calculateInner(addr_t now, std::vector<std::bitset<N>>& res){
    if(now == 0){ // bottom terminal
      return;
    }
    if(now == 1){ // top terminal
      size_t slen = skiplist.size();
      for(int k=0; k<(1<<slen); ++k){
        for(int i=0; i<slen; ++i){
          base[skiplist[i]] = static_cast<bool>(k & (1 << i));
        }
        res.emplace_back(base);
      }
      return;
    }
    addr_t now0 = B.loid(now);
    addr_t now1 = B.hiid(now);
    int nowlv = B.level(now);
    int now0lv = B.level(now0);
    int now1lv = B.level(now1);
    
    // see 0-child
    base[nowlv] = false;
    for(int i=nowlv+1; i<now0lv; ++i){
      skiplist.emplace_back(i);
    }
    calculateInner(now0, res);
    for(int i=nowlv+1; i<now0lv; ++i){
      skiplist.pop_back();
    }
    
    // see 1-child
    base[nowlv] = true;
    for(int i=nowlv+1; i<now1lv; ++i){
      skiplist.emplace_back(i);
    }
    calculateInner(now1, res);
    for(int i=nowlv+1; i<now1lv; ++i){
      skiplist.pop_back();
    }
  }
};

#endif // VARIANCE_BDDENUMERATE_HPP

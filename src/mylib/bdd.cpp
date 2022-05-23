#include "common.hpp"
#include "graph.hpp"
#include "bdd.hpp"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <unordered_map>

bool BDD::readfromFile(const char *filename){
  FILE *fp;
  std::unordered_map<addr_t, addr_t> IDToPos;
  if((fp = fopen(filename, "r")) == NULL){
    return false;
  }
  int num_vars = 0;
  for(int i=0; i<3; ++i){
    char s1[32];
    int tmp;
    fscanf(fp, "%s %d\n", s1, &tmp);
    if(s1[1] == 'i') num_vars = tmp;
  }
  nodes.emplace_back(num_vars, -1, -1); // pos = 0 -> FALSE
  nodes.emplace_back(num_vars, -1, -1); // pos = 1 -> TRUE
  addr_t pos = 2;
  char buf[1024];
  while(fgets(buf, 1023, fp)){
    addr_t ID;
    int _lv;
    char s1[32],s2[32];
    if(sscanf(buf, "%lld %d %s %s", &ID, &_lv, s1, s2) != 4){
      break;
    }
    _lv = num_vars - _lv;
    addr_t _lo, _hi;
    IDToPos.emplace(ID, pos);
    switch(s1[0]){
    case 'T': _lo = 1LL; break;
    case 'F': _lo = 0LL; break;
    default: _lo = IDToPos.at(atoll(s1)); break;
    }
    switch(s2[0]){
    case 'T': _hi = 1LL; break;
    case 'F': _hi = 0LL; break;
    default: _hi = IDToPos.at(atoll(s2)); break;
    }
    nodes.emplace_back(_lv, _lo, _hi);
    ++pos;
  }
  fclose(fp);
  nodes.shrink_to_fit();
  return true;
}

int BDD::readOrderfromFile(const char *filename, const Graph& g){
  FILE *fp;
  if((fp = fopen(filename, "r")) == NULL){
    return 1;
  }
  int u, v;
  while(fscanf(fp, "%d%d", &u, &v) != EOF){
    int ord = g.etovar(u, v);
    if(ord < 0){
      fclose(fp);
      return 2;
    }
    vars.emplace_back(ord);
  }
  fclose(fp);
  vars.shrink_to_fit();
  
  size_t siz = vars.size();
  invvars.resize(siz);
  for(size_t i=0; i<siz; ++i){
    invvars[vars[i]] = i;
  }
  nodes[0].lv = nodes[1].lv = siz;
  return 0;
}

double BDD::expectProb(const std::vector<double>& p) const{
  size_t leng = nodes.size();
  size_t dim = vars.size();
  assert(dim == p.size());
  
  std::vector<double> dp(leng);
  dp[0] = 0.0;
  dp[1] = 1.0;
  for(size_t i=2; i<leng; ++i){
    int now_lv = nodes[i].lv;
    dp[i] = dp[nodes[i].lo] * (1.0 - p[vars[now_lv]]) + dp[nodes[i].hi] * p[vars[now_lv]];
  }
  return dp[leng-1];
}
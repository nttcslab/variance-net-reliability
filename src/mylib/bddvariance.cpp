#include "common.hpp"
#include "graph.hpp"
#include "bdd.hpp"
#include "bddvariance.hpp"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <utility>
#include <vector>
#include <unordered_map>

double BDDVariance::calculate(const std::vector<double>& _p, const std::vector<double>& _s){
  assert(dim == _p.size() && dim == _s.size());
  p.clear();
  p.reserve(dim);
  std::copy(_p.begin(), _p.end(), std::back_inserter(p));
  q.clear();
  q.reserve(dim);
  for(const auto& v : p) q.emplace_back(1.0 - v);
  s.clear();
  s.reserve(dim);
  std::copy(_s.begin(), _s.end(), std::back_inserter(s));
  eval.clear();
  eval.resize(leng);
  std::fill(eval.begin(), eval.end(), 0.0);
  vval.clear();
  
  // bottom-up probability calculation
  eval[0] = 0.0;
  eval[1] = 1.0;
  for(addr_t i=2; i<leng; ++i){
    int now_var = B.var(B.level(i));
    eval[i] = eval[B.loid(i)] * q[now_var] + eval[B.hiid(i)] * p[now_var];
  }
  
  // variance calculation (Apply-like manner)
  return calculateInner(leng-1, leng-1);
}

double BDDVariance::calculateInner(addr_t u, addr_t v){
  // base case: if either of u and v is terminal, covariance = 0
  if(u <= 1LL || v <= 1LL) return 0.0;
  
  // since Cov[R_u, R_v] = Cov[R_v, R_u], we can assume addr(u) <= addr(v)
  if(u > v) std::swap(u, v);
  
  // if already calculated, return cache value
  auto it = vval.find(std::make_pair(u, v));
  if(it != vval.end()){
    return it->second;
  }
  
  double res = 0.0;
  int now_var = B.var(B.level(v));
  if(B.level(u) == B.level(v)){  // u and v are at the same level
    double cu0v0 = calculateInner(B.loid(u), B.loid(v));
    double cu0v1 = calculateInner(B.loid(u), B.hiid(v));
    double cu1v0 = calculateInner(B.hiid(u), B.loid(v));
    double cu1v1 = calculateInner(B.hiid(u), B.hiid(v));
    res = (q[now_var] * q[now_var] + s[now_var]) * cu0v0
            + (p[now_var] * p[now_var] + s[now_var]) * cu1v1
            + s[now_var] * (eval[B.hiid(u)] - eval[B.loid(u)]) * (eval[B.hiid(v)] - eval[B.loid(v)])
            + (p[now_var] * q[now_var] - s[now_var]) * (cu0v1 + cu1v0);
  }else{                         // if not, v's level is higher than u's since addr(u) <= addr(v)
    double cuv0 = calculateInner(u, B.loid(v));
    double cuv1 = calculateInner(u, B.hiid(v));
    res = q[now_var] * cuv0 + p[now_var] * cuv1;
  }
  
  // add cache entry
  vval.emplace(std::make_pair(u, v), res);
  
  return res;
}

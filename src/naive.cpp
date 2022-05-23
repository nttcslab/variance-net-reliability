#include "mylib/common.hpp"
#include "mylib/graph.hpp"
#include "mylib/bdd.hpp"
#include "mylib/bddenumerate.hpp"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <cassert>
#include <chrono>
#include <bitset>
#include <iostream>

constexpr size_t BIT_SIZE = 64;

void print_usage(char *fil){
  fprintf(stderr, "Usage: %s [graph_file] [order_file] [bdd_file] [probability_file] <preprocessed_probability_file>\n", fil);
}

int main(int argc, char **argv){
  if(argc < 5){
    fprintf(stderr, "ERROR: too few arguments.\n");
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }
  
  Graph G;
  int n, m;
  int src;
  
  if(!(G.readfromFile(argv[1]))){
    fprintf(stderr, "[ERROR] Cannot open graph file: %s\n", argv[1]);
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }
  n = G.numV();
  m = G.numE();
  
  if (m > BIT_SIZE){
    fprintf(stderr, "[ERROR] Too many edges (limit: %zu).\n", BIT_SIZE);
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }
  
  BDD B;
  if(!(B.readfromFile(argv[3]))){
    fprintf(stderr, "[ERROR] Cannot open bdd file: %s\n", argv[3]);
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }
  
  if(int err = B.readOrderfromFile(argv[2], G)){
    switch(err){
    case 1: fprintf(stderr, "[ERROR] Cannot open order file: %s\n", argv[2]);
    case 2: fprintf(stderr, "[ERROR] Order file %s contains non-exist edge.\n", argv[2]);
    }
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }
  
  std::vector<double> pi(m);
  std::vector<double> si(m);
  {
    FILE *fp;
    if((fp = fopen(argv[4], "r")) == NULL){
      fprintf(stderr, "ERROR: reading probability file %s failed.\n", argv[4]);
      print_usage(argv[0]);
      exit(EXIT_FAILURE);
    }
    
    for(size_t i=0; i<m; ++i){
      fscanf(fp, "%lf", &pi[i]);
    }
    for(size_t i=0; i<m; ++i){
      fscanf(fp, "%lf", &si[i]);
    }
    fclose(fp);
  }
  
  std::vector<double> ppi;
  std::vector<double> psi;
  size_t psize = 0;
  
  if(argc > 5){
    FILE *fp;
    if((fp = fopen(argv[5], "r")) == NULL){
      fprintf(stderr, "ERROR: reading preprocessed probability file %s failed.\n", argv[5]);
      print_usage(argv[0]);
      exit(EXIT_FAILURE);
    }
    double tmp;
    while(fscanf(fp, "%lf", &tmp) != EOF){
      ppi.emplace_back(tmp);
    }
    fclose(fp);
    
    psize = ppi.size() / 2;
    psi.reserve(psize);
    for(size_t i=psize; i<psize*2; ++i){
      psi.emplace_back(ppi[i]);
    }
    ppi.resize(psize);
    ppi.shrink_to_fit();
  }
  
  auto cstart = std::chrono::system_clock::now();
  
  double expect;
  if(B.size() > 2) expect = B.expectProb(pi);
  else             expect = 1.0;
  
  auto eend = std::chrono::system_clock::now();
  double etime = std::chrono::duration_cast<std::chrono::milliseconds>(eend-cstart).count();
  auto lend = std::chrono::system_clock::now();
  double ltime = std::chrono::duration_cast<std::chrono::milliseconds>(lend-eend).count();
  double variance;
  size_t bsiz = B.size();
  if(B.size() > 2){
    std::vector<std::bitset<BIT_SIZE>> model;
    BDDEnumerate<BIT_SIZE> BE(B);
    BE.calculate(model);
    model.shrink_to_fit();
    lend = std::chrono::system_clock::now();
    ltime = std::chrono::duration_cast<std::chrono::milliseconds>(lend-eend).count();
    variance = 0.0;
    std::vector<double> pi_ord;
    std::vector<double> si_ord;
    pi_ord.resize(m);
    si_ord.resize(m);
    for(int i=0; i<m; ++i){
      pi_ord[i] = pi[B.var(i)];
      si_ord[i] = si[B.var(i)];
    }
    size_t msize = model.size();
    fprintf(stderr, "model=%zu\n", msize);
    for(size_t i=0; i<msize; ++i){
      for(size_t j=i; j<msize; ++j){
        const std::bitset<BIT_SIZE>& u = model[i];
        const std::bitset<BIT_SIZE>& v = model[j];
        std::bitset<64> x = (u ^ v);
        double te = 1.0;
        double tv = 0.0;
        for(int i=0; i<m; ++i){
          double tpe = (u[i] ? pi_ord[i] : 1.0 - pi_ord[i]) * (v[i] ? pi_ord[i] : 1.0 - pi_ord[i]);
          double tpv = (x[i] ? -si_ord[i] : si_ord[i]);
          tv = (te + tv) * tpv + tv * tpe;
          te *= tpe;
        }
        variance += tv * (i == j ? 1 : 2);
      }
    }
  }else variance = 0.0;
  
  for(size_t i=0; i<psize; ++i){
    variance = (ppi[i] * ppi[i] + psi[i]) * variance + psi[i] * expect * expect;
    expect *= ppi[i];
  }
  
  auto cend = std::chrono::system_clock::now();
  
  double vtime = std::chrono::duration_cast<std::chrono::milliseconds>(cend-lend).count();
  fprintf(stderr, "%zu,%.lf,%.lf,%.lf\n", bsiz, etime, ltime, vtime); 

  printf("%.15le,%.15le,%.15le\n", expect, variance, sqrt(variance));
  
  return 0;
}

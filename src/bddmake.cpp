#include "mylib/common.hpp"
#include "mylib/graph.hpp"

#include "tdzdd/DdSpec.hpp"
#include "tdzdd/DdStructure.hpp"
#include "tdzdd/spec/FrontierBasedSearch.hpp"
#include "tdzdd/util/Graph.hpp"

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cassert>
#include <unordered_set>
#include <chrono>
#include <iostream>
#include <fstream>

void print_usage(char *fil){
  fprintf(stderr, "Usage: %s [graph_file] [source_vertex] [order_file] [out_bdd_file]\n", fil);
}

int main(int argc, char **argv){
  if(argc < 5){
    fprintf(stderr, "ERROR: too few arguments.\n");
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }
  
  Graph G;
  int n, m;
  std::unordered_set<int> srcs;

  {
    Graph H;
    if(!H.readfromFile(argv[1])){
      fprintf(stderr, "ERROR: reading graph file %s failed.\n", argv[1]);
      print_usage(argv[0]);
      exit(EXIT_FAILURE);
    }
    
    n = H.numV();
    m = H.numE();
    
    {
      FILE *fp;
      if((fp = fopen(argv[2], "r")) == NULL){
        fprintf(stderr, "ERROR: reading source vertices file %s failed.\n", argv[2]);
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
      }
      
      int src;
      while(fscanf(fp, "%d", &src) != EOF){
        srcs.emplace(src);
      }
      fclose(fp);
    }
    
    if(!G.readfromFile(argv[3])){
      fprintf(stderr, "ERROR: reading order file %s failed.\n", argv[3]);
      print_usage(argv[0]);
      exit(EXIT_FAILURE);
    }
  }
  
  if(srcs.size() <= 1){
    FILE *fp;
    if((fp = fopen(argv[4], "w")) == NULL){
      exit(EXIT_FAILURE);
    }
    fprintf(fp, "_i %d\n", m);
    fprintf(fp, "_o 1\n");
    fprintf(fp, "_n 0\n");
    fprintf(fp, "T\n");
    fclose(fp);
    return 0;
  }
  
  auto cstart = std::chrono::system_clock::now();
  
  tdzdd::Graph tG;
  
  for(const auto& edg : G.e){
    tG.addEdge(std::to_string(edg.first), std::to_string(edg.second));
  }
  tG.update();

  for(const auto& src : srcs){
    tG.setColor(std::to_string(src), 1);
  }
  tG.update();
  
  tdzdd::FrontierBasedSearch fbs(tG, -1, false, false);
  
  tdzdd::DdStructure<2> dd(fbs);
  dd.bddReduce();
  
  auto pend = std::chrono::system_clock::now();
  double ptime = std::chrono::duration_cast<std::chrono::milliseconds>(pend-cstart).count();
  fprintf(stderr, "%.6lf ms\n", ptime);
  
  std::ofstream ofs(argv[4]);
  if(!ofs){
    fprintf(stderr, "ERROR: opening output file %s failed.\n", argv[4]);
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }
  dd.dumpSapporo(ofs);
  
  return 0;
}
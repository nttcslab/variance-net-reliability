#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <utility>
#include <algorithm>
#include <string>
#include <unordered_set>
#include <unordered_map>

/*
argv[1]: input graph
argv[2]: input terminal
argv[3]: input probability and variance
argv[4]: output graph
argv[5]: output terminal
argv[6]: output probability and variance
argv[7]: output multiplicant probability and variance
*/
int main(int argc, char **argv){
  int m = 0;
  int n = 0;
  std::vector<std::pair<int, int>> edges;
  
  if(argc <= 7){
    exit(EXIT_FAILURE);
  }
  
  FILE *fp;
  
  int tmp1, tmp2;
  if((fp = fopen(argv[1], "r")) == NULL){
    exit(EXIT_FAILURE);
  }
  while(fscanf(fp, "%d%d", &tmp1, &tmp2) != EOF){
    n = std::max(n, tmp1);
    n = std::max(n, tmp2);
    ++m;
    edges.emplace_back(tmp1, tmp2);
  }
  fclose(fp);
  
  std::unordered_set<int> terms;
  if((fp = fopen(argv[2], "r")) == NULL){
    exit(EXIT_FAILURE);
  }
  while(fscanf(fp, "%d", &tmp1) != EOF){
    terms.emplace(tmp1);
  }
  fclose(fp);
  
  std::vector<std::string> plist, vlist;
  if((fp = fopen(argv[3], "r")) == NULL){
    exit(EXIT_FAILURE);
  }
  char tmps[80];
  for(int i=0; i<m; ++i){
    fscanf(fp, "%s", tmps);
    plist.emplace_back(tmps);
  }
  for(int i=0; i<m; ++i){
    fscanf(fp, "%s", tmps);
    vlist.emplace_back(tmps);
  }
  fclose(fp);
  
  int newm = m;
  
  std::vector<std::unordered_set<int>> ids(n+1);
  std::vector<int> flg(m, 1);
  std::vector<std::string> mplist, mvlist;
  for(int i=0; i<m; ++i){
    ids[edges[i].first].emplace(i);
    ids[edges[i].second].emplace(i);
  }
  
  bool updated = false;
  do{
    updated = false;
    for(int i=1; i<=n; ++i){
      if(ids[i].size() == 1){
        updated = true;
        int eid = *(ids[i].begin());
        flg[eid] = 0;
        ids[edges[eid].first].erase(eid);
        ids[edges[eid].second].erase(eid);
        --newm;
        auto it = terms.find(i);
        if(it != terms.end()){
          terms.erase(it);
          if(edges[eid].first == i) terms.emplace(edges[eid].second);
          else                      terms.emplace(edges[eid].first);
          mplist.emplace_back(plist[eid]);
          mvlist.emplace_back(vlist[eid]);
        }
      }
    }
  }while(updated);
  
  int cnt = 0;
  std::vector<int> renum(n+1, 0);
  for(int i=1; i<=n; ++i){
    if(ids[i].size() >= 2){
      renum[i] = ++cnt;
    }
  }
  
  if((fp = fopen(argv[4], "w")) == NULL){
    exit(EXIT_FAILURE);
  }
  fprintf(fp, "%d %d\n", cnt, newm);
  for(int i=0; i<m; ++i){
    if(flg[i]){
      int tmp1 = renum[edges[i].first];
      int tmp2 = renum[edges[i].second];
      if (tmp1 > tmp2) std::swap(tmp1, tmp2);
      fprintf(fp, "%d %d\n", tmp1, tmp2);
    }
  }
  fclose(fp);
  
  if((fp = fopen(argv[5], "w")) == NULL){
    exit(EXIT_FAILURE);
  }
  {
    std::vector<int> tlist;
    for(int i : terms) tlist.emplace_back(renum[i]);
    std::sort(tlist.begin(), tlist.end());
    for(int i : tlist) fprintf(fp, "%d\n", i);
  }
  fclose(fp);
  
  if((fp = fopen(argv[6], "w")) == NULL){
    exit(EXIT_FAILURE);
  }
  for(int i=0; i<m; ++i){
    if(flg[i]){
      fprintf(fp, "%s\n", plist[i].c_str());
    }
  }
  for(int i=0; i<m; ++i){
    if(flg[i]){
      fprintf(fp, "%s\n", vlist[i].c_str());
    }
  }
  fclose(fp);
  
  if((fp = fopen(argv[7], "w")) == NULL){
    exit(EXIT_FAILURE);
  }
  for(size_t i=0; i<mplist.size(); ++i){
    fprintf(fp, "%s\n", mplist[i].c_str());
  }
  for(size_t i=0; i<mvlist.size(); ++i){
    fprintf(fp, "%s\n", mvlist[i].c_str());
  }
  fclose(fp);
  
  return 0;
}
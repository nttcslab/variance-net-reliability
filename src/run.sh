#!/bin/sh

# 1st argument: graph file (.txt)
# 2nd argument: terminal file (.src)
# 3rd argument: probability file (.prob)
# 4th argument: preprocessed graph file (.prep)
# 5th argument: BDD order file (.prep.order)
./release/deldeg1 $1 $2 $3 /dev/null tmp.src tmp.prob tmp.mul
./release/bddmake $4 tmp.src $5 tmp.bdd >/dev/null 2>/dev/null
./release/main $4 $5 tmp.bdd tmp.prob tmp.mul
rm tmp.*

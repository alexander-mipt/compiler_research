# compiler_research
My IR experiments &amp; compiler optimizations

## Progress
* SSA - done
* CFG - done
* DFS & RPO - done
* Loop analyzer - done
  
* DF optimizations - TODO

## Build
```bash
mkdir build && cd build
make
```
## Run tests
```bash
./tests/graph_test [DOM_TREE_1]
./tests/graph_test [DOM_TREE_2]
./tests/graph_test [DOM_TREE_3]
./tests/graph_test [LOOPS_1]
```
> All this tests print input SSA graph, some log info and output graph (e.g. loop tree or dom tree).


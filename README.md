# Impact of link availability uncertainty on network reliability: analyses with variances

This repository includes the codes to reproduce the results of experiments in the paper "Impact of link availability uncertainty on network reliability: analyses with variances" published in the proceedings of [2022 IEEE International Conference on Communications (ICC 2022)](https://icc2022.ieee-icc.org/).

## Requirements

All codes are written in C++11 language. Before building our code, you must place the header files of [TdZdd](https://github.com/kunisura/TdZdd) into `src/` directory. More specifically, all header files of [TdZdd/include/tdzdd](https://github.com/kunisura/TdZdd/tree/master/include/tdzdd) must be placed on `src/tdzdd/` directory; e.g. `src/tdzdd/DdEval.hpp`, `src/tdzdd/DdSpec.hpp`, etc.

After installing TdZdd, if your environment has CMake version >=3.8, you can build all codes with the following command:

```shell
(moving to src/ directory)
mkdir release
cd release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

After running this, all binaries are generated at the `release/` directory.

### Verified environments

We verified that the building process of our codes and the commands presented below worked fine in the following macOS and Linux environments:

- macOS Big Sur 11.2.1 + Apple clang 12.0.0
- CentOS 7.9 + gcc 4.8.5

## How to reproduce experimental results

After building our code, the following 4 binaries are generated: `main`, `naive`, `bddmake`, and `deldeg1`. `main` implements our proposed method, while `naive` implements the naive method. `bddmake` is a program to build a BDD, and `deldeg1` is a program to perform a preprocessing described in Section III-D of our paper.

All data used in our experiments are in `data.tar.gz`. After extracting, it includes the data of the following graphs:
- Grid graphs: `grid4x4.txt` (Grid-3x3), `grid5x5.txt` (Grid-5x5), `grid6x6.txt` (Grid-6x6), `grid7x7.txt` (Grid-7x7), `grid7x<h>.txt` (Grid-7x`<h>`)
- [Topology Zoo](http://www.topology-zoo.org/index.html): `0146-real-Interoute.txt` (Interoute), `0146-real-Ion.txt` (Ion), `0151-real-DialtelecomCz.txt` (DialtelecomCz), `0161-real-Deltacom.txt` (Deltacom), `0186-real-TataNld.txt` (TataNld), `0189-real-UsCarrier.txt` (UsCarrier)

For each graph, the following data are included:

- `<graphname>`: the link list of the graph. Used as `[graph_file]`.
- `<graphname>.<k>.<i>.src`: the list of randomly selected `<k>` vertices. Used as `[terminal_file]`.
- `<graphname>.prob`: the availability ($p_i$) of each link ($e_i$) in the same order as the link list, following the availability's variance ($\sigma_i^2$) of each link ($e_i$) in the same order. Used as `[probability_file]`.
- `<graphname>.prep`: the link list of the preprocessed graph. Used as `[preprocessed_graph_file]`.
- `<graphname>.prep.order`: the link ordering (determined as described in our paper) of the preprocessed graph. Used as `[preprocessed_order_file]`.

### Experiments

To check the computational cost of our method for computing VoR, run:

```shell
./run.sh [graph_file] [terminal_file] [probability_file] [preprocessed_graph_file] [preprocessed_order_file]
```

After execution, the program generates one-line output to `stderr`, containing (i) the BDD size, (ii) the elapsed time (in milliseconds) for computing the expectation of network reliability, and (iii) the elapsed time (in milliseconds) for computing VoR.
Also, the program generates one-line output to `stdout`, containing (i) the expectation of network reliability, (ii) VoR value, and (iii) the square root of VoR.

The naive method can also be executed as follows:

```shell
./runnaive.sh [graph_file] [terminal_file] [probability_file] [preprocessed_graph_file] [preprocessed_order_file]
```

Note that the naive method also uses BDD to represent all the states connecting terminals, but all the states are explicitly enumerated using this BDD before computing VoR. Such method is generally faster than directly enumerating all the states connecting terminals from a graph and termimals.

After execution, the program generates one-line output to `stderr`, containing (i) the BDD size, (ii) the elapsed time (in milliseconds) for computing the expectation of network reliability, (iii) the elapsed time (in milliseconds) for enumerating all the states, and (iv) the elapsed time (in milliseconds) for computing VoR.
Also, the program generates one-line output to `stdout`, containing (i) the expectation of network reliability, (ii) VoR value, and (iii) the square root of VoR.

_Running example:_ To execute our proposed method for the Grid-6x6 graph with half-terminals setting, run:

```shell
./run.sh data/grid6x6.txt data.grid6x6.txt.18.<i>.src data/grid6x6.txt.prob data/grid6x6.txt.prep data/grid6x6.txt.prep.order
```

, where `<i>` ranges from 1 to 20.

## License

This software is released under the NTT license, see `LICENSE.txt`.

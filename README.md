# OMP lab-1
---

## Description

This program is intended to show the scopes of multi-threaded programming during mathematical computations using OpenMP. For instance, calculation of a numerical integral by centered rectangles method is used as the workload function. For the integration step determining purposes the epsilon value is used according to the Runge's rule.

## Compilation and usage

In order to build the program you have to run `make` command within the root folder. By default Makefile uses `clang++` and `libomp-dev`, path to lib must be stored in the `$LIBRARY_PATH` environment variable, otherwise successful compilation might not be guaranteed.

Basic use-case scenario for this program is to run it as
```
$ ./build/omp_lab1.elf <in_file> <out_file> <num_threads>
```

Aforementioned input file must contatin information in format
```
<left_bound> <right_bound> <epsilon>
```

## Test suite

The entire test suite is written in bash and performs black-box tests as well as time-measurements. It is enough to run the test suite once with a single script `tests/run_test_suite.bash` because it checks the correct folder structure and installs required modules. After that you can execute any tests as needed. Recommended folder structure with tests is shortly shown below.

```
data
    `-- img
tests
    |-- run_test_suite.bash             ### Test suite runner
    |-- perf_tests.bash                 ### Performance measurement
    `-- sanity_check_black-box.bash     ### Functional testing
tools
    `-- build_charts.py                 ### Charts builder
```

## Performance tests

These tests were made on different machines, one of which is a high-end 8-core modern CPU, and the other is an obsolete low-end 2-core CPU. The results are equally consistent on both of them. All charts are shown in comparison of the 8-core on the left side and the 2-core on the right side.

### Performance / workload (different thread configs)

Here is the first comparison, that shows dependency of performance on the amount of workload giving different configurations of threads; both axes are in logarithmic scale. Performance is measured as time, less is better. Workload is represented as Epsilon, smaller Epsilon increases the time according to Runge's rule.

Lesser number of threads predictably gives better results with a small amount of workload, but gets worse as workload grows. Dynamic scheduling expenses are huge, hence, considering small time of a single operation, it performs exponentially worse than the single thread. Thus, static scheduling is a favorite comparing to others.

<p float="left">
    <img src="data/img/Performance from workload.png" width="320"/>
    <img src="data/img/E2_Performance from workload.png" width="320"/>
</p>

### Performance / chunk_size (heavy workload)

By passing a heavy workload, a bar chart of performance over a chunk size can be built.
Here we can see that the default chunk size (chunk_inf) performs better than the others. Similar result appears to be true on both a high-end and a low-end CPU's.

<p float="left">
    <img src="data/img/Performance from chunk_size.png" width="320"/>
    <img src="data/img/E2_Performance from chunk_size.png" width="320"/>
</p>

### Performance / workload (different chunk_size)

Repeating the results of a chart above dynamically we are getting changes in performance over a workload also depending on the size of a chunk. Logarithmic scale on both axes.

<p float="left">
    <img src="data/img/Performance from chunk_size and workload.png" width="320"/>
    <img src="data/img/E2_Performance from chunk_size and workload.png" width="320"/>
</p>

---
#### ITMO University, spring of 2022
# High Performance Computing

Versionning of IS1240 course assignments back in 2016 at Centrale Paris.

## Why?

Exercises are intended to learn how to write programs using [MPI](https://en.wikipedia.org/wiki/Message_Passing_Interface)
and [Open MPI](https://www.open-mpi.org/).

Some directories are solutions for https://www.mcs.anl.gov/research/projects/mpi/tutorial/mpiexmpl/contents.html.

Other are course assignments and reports.

## How?

### Compile

```bash
mpicc -Wall main.c -o main.out
```

### Run

```bash
mpirun -np 4 main.out
```

## Where?

Further resources are available at [mpitutorial.com](http://mpitutorial.com/tutorials/).

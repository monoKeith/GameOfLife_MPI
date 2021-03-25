# GameOfLife_MPI
Distributed computing (MPI) implementation of GameOfLife.


## How to compile:

    make


## How to run:

    run <N> <k> <m> <input_file>


## Change number of processors

1.

Edit the run script, in this line:

    mpiexec -np 9 --hostfile hostfile main "$1" "$2" "$3" "$4"

change the argument followed by -np to any squared number (e.g. 2, 4, 9, 16, 25).


2.

Or, just run the program with mpiexec without using the run script:

    mpiexec -np 9 --hostfile hostfile main <N> <k> <m> <input_file>
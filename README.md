# GameOfLife_MPI
Distributed computing (MPI) implementation of GameOfLife.


## How to compile:

    make


## How to run:

    ./run <N> <k> <m> <input_file>

## Output file:

The name of the output file depends on the the number m, which determines how often an output is generated.

The name of output file is:

    iteration_<iteration_number>.txt


## Change number of processors

1. Edit the run script, in this line:

    mpiexec -np 9 --hostfile hostfile main "$1" "$2" "$3" "$4"

change the argument followed by -np to any squared number (e.g. 1, 4, 9, 16, 25).


2. Or, just run the program with mpiexec without using the run script:

    mpiexec -np <num_processor> --hostfile hostfile main <N> <k> <m> <input_file>


## MPI host settings

Edit the hostfile, add hostname or ip address.


## Some runtime measurements

Measured with 4 virtual machines, each with a dual-core processor and 2GB of RAM.

    ./run 100 10 10 ./test_files/test\ 1\ input\ \(N\=100\).txt

* Test 1
    * 1  Processor: 0.01554351s
    * 4  Processor: 0.05873571s
    * 16 Processor: 0.21973049s


    ./run 100 10 10 ./test_files/test\ 2\ input\ \(N\=100\).txt

* Test 2
    * 1  Processor: 0.01860844s
    * 4  Processor: 0.10314720s
    * 16 Processor: 0.17839387s


    ./run 100 10 10 ./test_files/test\ 3\ input\ \(N\=100\).txt

* Test 3
    * 1  Processor: 0.01890066s
    * 4  Processor: 0.05995084s
    * 16 Processor: 0.20624947s

Since the calculation size is too small, overhead of communication is very high.

#include "filehandle.hpp"
#include "piece.hpp"
#include "process.hpp"
#include "rootprocess.hpp"
#include "mpi.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <math.h>
using namespace std;

// Global vars
const int ROOT_ID = 0;
int N, k, m;
int PROCESSOR_ID;
int PROCESSOR_COUNT;


int main(int argc, char* argv[])
{
    //  Initialize MPI.
    MPI::Init(argc, argv);

    // Number of processes & individual process ID.
    PROCESSOR_COUNT = MPI::COMM_WORLD.Get_size();
    PROCESSOR_ID = MPI::COMM_WORLD.Get_rank();

    if (PROCESSOR_ID == 0) {
        // Host: Error checking & parse arguments.
        if (argc != 5) {
            cout << "ARGC = " << argc << endl;
            cout << "Usage: run <N> <k> <m> <file name>" << endl;
            MPI_Abort(MPI::COMM_WORLD, 1);
        }

        // Check if process count supported.
        float numPart = sqrt(PROCESSOR_COUNT);
        if (ceil(numPart) > numPart) {
            cout << "Processor count unsupported! Should be a square! (e.g. 2, 4, 8, 16, 32)" << endl;
            MPI_Abort(MPI::COMM_WORLD, 1);
        }

        // Run host
        RootProcess process(argv);
        process.run();
    } else {
        // Run dist
        Process process;
        process.run();
    }

    // Terminate MPI.
    MPI::Finalize();

    if (PROCESSOR_ID == 0) {
        cout << "Done." << endl;
    }

    return 0;
}

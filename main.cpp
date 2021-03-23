
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


void run_host(char* argv[])
{
    RootProcess process(argv);
    process.distributeWork();
    process.initialize();
    process.iterate();
}


void run_dist()
{
    Process process;
    process.initialize();
    process.iterate();
}


int main(int argc, char* argv[])
{
    double wtime = MPI::Wtime();

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
        run_host(argv);
    } else {
        // Run dist
        run_dist();
    }

    // Terminate MPI.
    MPI::Finalize();
    wtime = MPI::Wtime() - wtime;

    if (PROCESSOR_ID == 0) {
        cout << "  Elapsed wall clock time = " << wtime << " seconds.\n";
    }

    return 0;
}

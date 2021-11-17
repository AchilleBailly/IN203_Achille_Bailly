#include <cstdlib>
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <mpi.h>
#include <cmath>

unsigned log2_int(int x)
{
	static unsigned result;
	for (int i = 0; (2 ^ i) < x; i++)
		result = i;
	assert((2 ^ result) == x); // x must be a power of 2
	return result;
}

int main(int nargs, char *argv[])
{
	MPI_Init(&nargs, &argv);
	MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
	int nbp;
	MPI_Comm_size(globComm, &nbp);
	int rank;
	MPI_Comm_rank(globComm, &rank);
	// Création d'un fichier pour ma propre sortie en écriture :
	// std::stringstream fileName;
	// fileName << "Output" << std::setfill('0') << std::setw(5) << rank << ".txt";
	// std::ofstream output( fileName.str().c_str() );

	unsigned dim = log2(nbp);

	int token = 42;

	int buffer;
	if (rank == 0)
	{
		buffer = token;
		std::cout << "Process 0 emitted token " << token << "\n";
	}

	for (int i = 0; i < dim; i++)
	{
		MPI_Status status;
		if (rank >= (int)exp2(i) && rank < (int)exp2(i + 1))
		{
			MPI_Recv(&buffer, 1, MPI_INT, rank - (int)exp2(i), i, globComm, &status);
			std::cout << "Iteration " << i + 1 << "; process " << rank << " received : " << buffer << "\n";
		}

		else if (rank < (int)exp2(i))
		{
			MPI_Send(&buffer, 1, MPI_INT, rank + (int)exp2(i), i, globComm);
		}
	}

	// output.close();
	MPI_Finalize();
	return EXIT_SUCCESS;
}

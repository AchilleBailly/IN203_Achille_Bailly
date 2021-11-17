#include <iostream>
#include <cstdlib>
#include <mpi.h>

int main(int nargs, char *argv[])
{
	// On initialise le contexte MPI qui va s'occuper :
	//    1. Créer un communicateur global, COMM_WORLD qui permet de gérer
	//       et assurer la cohésion de l'ensemble des processus créés par MPI;
	//    2. d'attribuer à chaque processus un identifiant ( entier ) unique pour
	//       le communicateur COMM_WORLD
	//    3. etc...

	MPI_Init(&nargs, &argv);
	MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
	int nbp;
	MPI_Comm_size(globComm, &nbp);
	int rank;
	MPI_Comm_rank(globComm, &rank);

	if (rank == 0)
	{
		int token = 42;
		MPI_Send(&token, 1, MPI_INT, 1, 2, globComm);
		std::cout << "Process 0 sent the first token with value " << token << "\n";
	}
	else if (rank == nbp - 1)
	{
		int token;
		MPI_Status status;
		MPI_Recv(&token, 1, MPI_INT, rank - 1, 2, globComm, &status);
		std::cout << "Last process number " << rank << " received the token with value " << token << "\n";
	}
	else
	{
		int token;
		MPI_Status status;
		MPI_Recv(&token, 1, MPI_INT, rank - 1, 2, globComm, &status);
		token++;
		std::cout << "Process " << rank << " sent token with value " << token << "\n";
		MPI_Send(&token, 1, MPI_INT, rank + 1, 2, globComm);
	}

	// A la fin du programme, on doit synchroniser une dernière fois tous les processus
	// afin qu'aucun processus ne se termine pendant que d'autres processus continue à
	// tourner. Si on oublie cet instruction, on aura une plantage assuré des processus
	// qui ne seront pas encore terminés.
	MPI_Finalize();
	return EXIT_SUCCESS;
}

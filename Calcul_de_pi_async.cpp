#include <chrono>
#include <random>
#include <cstdlib>
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <mpi.h>

// Attention , ne marche qu'en C++ 11 ou supérieur :
double approximate_pi(unsigned long nbSamples)
{
	typedef std::chrono::high_resolution_clock myclock;
	myclock::time_point beginning = myclock::now();
	myclock::duration d = beginning.time_since_epoch();
	unsigned seed = d.count();
	std::default_random_engine generator(seed);
	std::uniform_real_distribution<double> distribution(-1.0, 1.0);
	unsigned long nbDarts = 0;
	// Throw nbSamples darts in the unit square [-1 :1] x [-1 :1]
	for (unsigned sample = 0; sample < nbSamples; ++sample)
	{
		double x = distribution(generator);
		double y = distribution(generator);
		// Test if the dart is in the unit disk
		if (x * x + y * y <= 1)
			nbDarts++;
	}
	// Number of nbDarts throwed in the unit disk
	double ratio = double(nbDarts) / double(nbSamples);
	return 4 * ratio;
}

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
	int nbp = 0;
	MPI_Comm_size(globComm, &nbp);
	int rank = 0;
	MPI_Comm_rank(globComm, &rank);

	if (nbp == 0)
		return EXIT_FAILURE;

	int nbSamples = 10000000;

	// only used by process 0, the main process
	typedef std::chrono::steady_clock mychrono;
	mychrono ck;
	mychrono::time_point start = ck.now();

	//calculate this process ratio
	int processSamples = nbSamples / nbp;
	double ratio = approximate_pi(processSamples);

	//send it
	if (rank != 0)
	{
		MPI_Request r;
		MPI_Isend(&ratio, 1, MPI_DOUBLE, 0, 1, globComm, &r);
		std::cout << "Ratio for process " << rank << " calculated and sent !\n";
	}
	else if (rank == 0)
	{
		//process 0 calculates the mean value
		MPI_Request reqs[nbp - 1];
		MPI_Status status[nbp - 1];
		double to_rcv[nbp - 1];
		for (int i = 1; i < nbp; i++)
		{
			double rcv_ratio = 0;
			MPI_Irecv(&rcv_ratio, 1, MPI_DOUBLE, i, 1, globComm, &reqs[i - 1]);
			to_rcv[i - 1] = rcv_ratio;
		}
		MPI_Waitall(nbp - 1, reqs, status);
		for (int i = 1; i < nbp; i++)
		{
			ratio += to_rcv[i - 1];
		}
		ratio /= nbp;

		mychrono::time_point end = ck.now();
		std::chrono::duration<double, std::milli> dur = end - start;
		std::cout << "Process 0 : Value calculated for pi : " << ratio << " in " << dur.count() << " ms.\n";
	}

	// A la fin du programme, on doit synchroniser une dernière fois tous les processus
	// afin qu'aucun processus ne se termine pendant que d'autres processus continue à
	// tourner. Si on oublie cet instruction, on aura une plantage assuré des processus
	// qui ne seront pas encore terminés.
	MPI_Finalize();
	return EXIT_SUCCESS;
}

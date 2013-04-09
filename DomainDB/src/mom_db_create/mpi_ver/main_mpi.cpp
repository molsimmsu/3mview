#include "pdb_mpi.h"
#include <ctime>

#define _DEBUG_

int main(int args, char ** argv)
{
#ifdef _DEBUG_ 
	clock_t now = clock();
#endif
	if (args<2)
	{
		printf("No argument specified!\n");
		exit(-1);
	}
	PDB temp(argv[1]);
	temp.WriteMoments();

#ifdef _DEBUG_ 
	if (temp.rank == 0)
		printf("\ntime = %.2lf seconds\n", ((double)(clock()-now))/CLOCKS_PER_SEC);
#endif
	return 0;
}

#include "pdb.h"
#include <ctime>

int main(int args, char ** argv)
{
	clock_t now = clock();
	if (args<2)
	{
		printf("No argument specified!\n");
		exit(-1);
	}
	PDB temp(argv[1]);
	temp.WriteMoments();
	if (temp.rank == 0)
		printf("\ntime = %.2lf seconds\n", ((double)(clock()-now))/CLOCKS_PER_SEC);
	return 0;
}

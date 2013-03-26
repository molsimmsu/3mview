#include "pdb.h"
#include <ctime>

int main(int args, char ** argv)
{
	if (args<2)
	{
		printf("No argument specified!\n");
		exit(-1);
	}
	clock_t now = clock();
	PDB temp(argv[1]);
	temp.Reduce();
	temp.WriteMoments();
	printf("\ntime = %.2lf seconds\n", ((double)(clock()-now))/CLOCKS_PER_SEC);
	return 0;
}

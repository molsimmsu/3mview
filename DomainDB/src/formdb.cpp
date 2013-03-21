#include "pdb.h"

int main(int args, char ** argv)
{
  if (args<2)
	{
		printf("No argument specified!\n");
		exit(-1);
	}
	PDB temp(argv[1]);
	temp.Reduce();
	temp.WriteMoments();
	return 0;
}

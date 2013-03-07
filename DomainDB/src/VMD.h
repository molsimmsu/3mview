#ifndef VMD_H
#define VMD_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

typedef vector< vector<string> > VMDMolecules;

int colors[] = {
8,  // white
1,  // red
7,  // green
4,  // yellow
21, // cyan
27  // magenta
};

class VMD
{
private:

FILE* pipe;

void start()
{
	pipe = popen("vmd", "w");
	if (!pipe) cout << "Error!" << endl;
}

public:

bool loadMolecule(vector<string> mol)
{
	start();

	for (unsigned int i = 0; i < mol.size(); i++)
	{
		fprintf(pipe, "mol new %s;", mol[i].c_str());
		fprintf(pipe, "mol modcolor 0 %d ColorID %d;", i, colors[i]);
		fprintf(pipe, "mol modstyle 0 %d NewCartoon;", i);
	}
	fprintf(pipe, "mol new %s;", mol[0].c_str());
	fprintf(pipe, "\n");
	fflush(pipe);
	
	char c = getchar();
	pclose(pipe);
	
	return c != 27;
}

void loadMolecules(VMDMolecules mol)
{
	for (unsigned int i = 0; i < mol.size(); i++)
		if (!loadMolecule(mol[i])) return;
}

};

#endif

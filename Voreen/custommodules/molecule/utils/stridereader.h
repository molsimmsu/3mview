#ifndef STRIDEREADER_H
#define STRIDEREADER_H

#include <cstring>
#include <cstdio>
#include <string>
#include <vector>
using namespace std;

class StrideReader
{
public:
	StrideReader(const char* pdbPath);
	unsigned int numChains();
	unsigned int numResidues(unsigned int chainNum);
	char getResidueStructure(unsigned int chainNum, unsigned int residueNum);
	const vector< vector<char> >& getStructure();

private:
	vector< vector<char> > data;

	void parseLine(const char* str);
};

#endif // STRIDEREADER_H

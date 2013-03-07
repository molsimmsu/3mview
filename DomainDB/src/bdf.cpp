#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <tclap/CmdLine.h>
#include "Alignment.h"
using namespace std;
using namespace TCLAP;

void readPipe(string cmd)
{
	FILE* pipe = popen(cmd.c_str(), "r");
	char buf[100];

	while (!feof(pipe))
	{
		fgets(buf, 100, pipe);
		cout << buf;
	}

	pclose(pipe);
}

int main(int argc, char** argv)
{
try
{
CmdLine cmd("Find homologous domains by a given sequence", ' ', "1.0");

ValueArg<string> seqArg("f","find","Find sequence",true,"","sequence");
ValueArg<string> pipeArg("p", "pipe", "Pipeline", true, "", "pipe");
SwitchArg infoArg("i", "info", "Show database info", false);
SwitchArg makeArg("m", "make", "Make database", false);
SwitchArg cleanArg("c", "clean", "Clean database", false);

vector<Arg*> xorArgs;
xorArgs.push_back(&seqArg);
xorArgs.push_back(&pipeArg);
xorArgs.push_back(&infoArg);
xorArgs.push_back(&makeArg);
xorArgs.push_back(&cleanArg);
cmd.xorAdd(xorArgs);

cmd.parse(argc, argv);

if (cleanArg.isSet())
{
	system("rm -f DomainsDB/domains.phr DomainsDB/domains.pin");
	system("rm -f DomainsDB/domains.pog DomainsDB/domains.psd");
	system("rm -f DomainsDB/domains.psi DomainsDB/domains.psq");
	return 0;
}

if (makeArg.isSet())
{
	cout << "Creating databese" << endl;

	string cmd("./makeblastdb");
	cmd += " -in DomainsDB/domains.fa";
	cmd += " -dbtype prot";
	cmd += " -out DomainsDB/domains";
	cmd += " -title \"domains\"";
	cmd += " -parse_seqids";
	readPipe(cmd);
	
	return 0;
}

if (infoArg.isSet())
{
	string cmd("./blastdbcmd -info -db DomainsDB/domains");
	readPipe(cmd);
	return 0;
}

if (pipeArg.isSet())
{
	cout << pipeArg.getValue() << endl << endl;
	return 0;
}

if (seqArg.isSet())
{
	AlignmentList A;
	A.findHomologousBLAST(seqArg.getValue());
	A.sortByIdentityPercent();
	A.write(cout);

	return 0;
}

} catch (TCLAP::ArgException &e)
{ cerr << "error: " << e.error() << " for arg " << e.argId() << endl; }
}

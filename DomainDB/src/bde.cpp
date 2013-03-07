#include "DomainsDB.h"
#include "Molecule.h"
#include "VMD.h"
#include "Histogram.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <climits>
#include <cfloat>
#include <tclap/CmdLine.h>
using namespace std;
using namespace TCLAP;

string intToStr(int i)
{
	stringstream ss;
	ss << i;
	string str;
	ss >> str;
	return str;
}

vector<string> processMolecule(string fileName, DomainsDBConstraint c)
{
vector<string> files;

fileName.resize(5);

Molecule molecule;
molecule.readXYZ("pdb_DB/" + fileName + ".xyz");

// Попытка чтения, если соотв. *.seq файлы существуют.
// Если нет файлов с номерами 0 и 1, значит информации о домене нет
int failCount = 0, successCount = 0, i;

for (i = 0; ; i++)
{
	string nStr = intToStr(i);

	vector<Molecule> domainParts;
	string domainName = fileName + "_" + nStr;
	string domainFile = "seq_DOM/" + domainName + ".seq";
	
	if (Molecule::readDomain(domainFile, domainParts))
	{	
		Molecule result;
		
		for (unsigned int j = 0; j < domainParts.size(); j++)
			result.addMolecule(molecule.extractDomain(domainParts.at(j)));
		
		if (result.size() == 0) {
			cout << "Molecule size is zero: " << fileName << endl << endl;
			continue;
		}
		else if (c.satisfies(result)) {
			string domainPath = "DomainsDB/domains/" + domainName + ".pdb";
			files.push_back(domainPath);
			result.writePDB(domainPath);
			result.writeFASTA("DomainsDB/domains.fa", domainName.c_str());
			result.writeStat("DomainsDB/stat", domainName.c_str());
		}
		
		++successCount;
	}
	else ++failCount;

	if (failCount == 2) break;
	
}

string moleculePath = "DomainsDB/molecules/" + fileName + ".pdb";
files.insert(files.begin(), moleculePath);
molecule.writePDB(moleculePath);
	
if (i == 1) cout << "Domain files not found: " << fileName << endl;

return files;
}

void processInterval(int start, int end, DomainsDBConstraint c)
{
	FILE* files = popen("ls pdb_DB -1", "r");

	int n = 0;
	char line[10];

	while(fscanf(files, "%s\n", line) > 0) {
		string fileName(line);
		if (n >= start && n <= end) processMolecule(fileName, c);
		n++;
	}

	pclose(files);
}

void cleanFiles()
{
system("rm -f -r DomainsDB");
}

void initFiles()
{
system("mkdir DomainsDB");
system("mkdir DomainsDB/domains");
system("mkdir DomainsDB/histograms");
system("mkdir DomainsDB/molecules");
system("rm -f DomainsDB/domains.fa");
system("rm -f DomainsDB/stat");
}

DomainsDBConstraint setConstraints
	(ValueArg<double>& dmin, ValueArg<double>& dmax, 
	ValueArg<double>& asym3Min, ValueArg<double>& asym3Max,
	ValueArg<double>& asym2Min, ValueArg<double>& asym2Max)
{
	DomainsDBConstraint c;
	c.dmin = dmin.getValue();
	c.dmax = dmax.getValue();
	c.asym3Min = asym3Min.getValue();
	c.asym3Max = asym3Max.getValue();
	c.asym2Min = asym2Min.getValue();
	c.asym2Max = asym2Max.getValue();
	
	return c;
}

int main(int argc, char** argv)
{
CmdLine cmd("BALBES Domain Extractor", ' ', "1.0");

ValueArg<string> makeArg
	("m","make","Extract domains",true,"all","all|interval|name");
	
SwitchArg viewArg("v","view","View domains",true);
SwitchArg statArg("s","stat","Domains statistics",true);
SwitchArg cleanArg("c","clean","Clean domains",true);
	
vector<Arg*> xorlist;
xorlist.push_back(&viewArg);
xorlist.push_back(&makeArg);
xorlist.push_back(&statArg);
xorlist.push_back(&cleanArg);
cmd.xorAdd( xorlist );
	
ValueArg<int> firstArg
	("f","first","First domain in interval",false,0,"uint",cmd);
	
ValueArg<int> lastArg
	("l","last","Last domain in interval",false,INT_MAX,"uint",cmd);
	
ValueArg<double> dmin
	("","dmin","Min domain diameter",false,0,"double",cmd);
	
ValueArg<double> dmax
	("","dmax","Max domain diameter",false,DBL_MAX,"double",cmd);
	
ValueArg<double> asym3Min
	("","asym3Min","Min (J0-(j1+j2)/2)/J0 asymm",false,0,"double",cmd);
	
ValueArg<double> asym3Max
	("","asym3Max","Max (J0-(j1+j2)/2)/J0 asym",false,DBL_MAX,"double",cmd);
	
ValueArg<double> asym2Min
	("","asym2Min","Min ((J0-j2)/J0) asym",false,0,"double",cmd);
	
ValueArg<double> asym2Max
	("","asym2Max","Max ((J0-j2)/J0) asym",false,DBL_MAX,"double",cmd);
	
cmd.parse(argc, argv);

if (cleanArg.isSet())
{
	cleanFiles();
	return 0;
}

if (makeArg.isSet())
{
	cleanFiles();
	initFiles();
	
	DomainsDBConstraint c =
	setConstraints(dmin, dmax, asym3Min, asym3Max, asym2Min, asym2Max);

	if (makeArg.getValue() == "all")
	{
		processInterval(0, INT_MAX, c);
	}
	else
	if (makeArg.getValue() == "interval")
	{
		if (firstArg.isSet() && lastArg.isSet())
			processInterval(firstArg.getValue(), lastArg.getValue(), c);
	}
	else
	{
		vector<string> vmdFiles = processMolecule(makeArg.getValue(), c);
	}
}
else
if (viewArg.isSet())
{
	VMD vmd;
	DomainsDB db;
	// Создаем список молекул для отображения
	DomainsDBConstraint c = 
	setConstraints(dmin, dmax, asym3Min, asym3Max, asym2Min, asym2Max);
	
	vector<Domain> v = db.selectDomains(c);
	VMDMolecules mols = DomainsDB::domainsToMolecules(v);
	// По очереди открываем молекулы из созданного списка
	vmd.loadMolecules(mols);
}
else
if (statArg.isSet())
{
	DomainsDB db;
	// Создаем список молекул для отображения
	DomainsDBConstraint c = 
	setConstraints(dmin, dmax, asym3Min, asym3Max, asym2Min, asym2Max);
	
	vector<Domain> v = db.selectDomains(c);
	VMDMolecules mols = DomainsDB::domainsToMolecules(v);
	// По очереди открываем молекулы из созданного списка
	cout << v.size() << " domains" << endl;
	cout << mols.size() << " molecules" << endl;
	
	Histogram hist(v);
	hist.build(MoleculeParams::DIAMETER);
	hist.build(MoleculeParams::MASS);
	hist.build(MoleculeParams::NUM_ATOMS);
	hist.build(MoleculeParams::NUM_RESIDUES);
	hist.build(MoleculeParams::ASYM_2);	
	hist.build(MoleculeParams::ASYM_3);	
}
}

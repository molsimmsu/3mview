#ifndef MOLSIM_MOLECULE_H
#define MOLSIM_MOLECULE_H

#define RNUM 30

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <Eigen/Dense>
using namespace Eigen;
using namespace std;

class Atom
{
public:
string name, element;
double x, y, z;
double occupancy, tempFactor;

Atom(string name, string element, 
	double x, double y, double z, double tempFactor)
{
this->name = name;
this->element = element;
this->x = x;
this->y = y;
this->z = z;
this->tempFactor = tempFactor;
this->occupancy = 1.0;
}

double distance(Atom& A)
{
double dx = x - A.x;
double dy = y - A.y;
double dz = z - A.z;
return sqrt(dx*dx + dy*dy + dz*dz);
}

unsigned int atomMass()
{
if (element == "C") return 12;
if (element == "N") return 14;
if (element == "O") return 16;
if (element == "P") return 31;
if (element == "S") return 32;
if (element == "U") return 238;

cout << "Atomic mas not defined: " << element << endl;

return 0;
}

};

class Residue
{
public:
static string names[RNUM];
static char codes[RNUM];
static bool initialized;

string name;
vector<Atom> atoms;

Residue(string name)
{
this->name = name;
}

bool hasMainCarbon()
{
for (unsigned int i = 0; i < size(); i++)
{
	Atom a = getAtom(i);
	if (a.name.compare(0, 2, "CA", 2) == 0) return true;
}
return false;
}

Atom getMainCarbon()
{
for (unsigned int i = 0; i < size(); i++)
{
	Atom a = getAtom(i);
	if (a.name.compare(0, 2, "CA", 2) == 0) return a;
}
cout << "ERROR: Main carbon not found" << endl;
exit(1);
}

void addAtom(Atom a)
{
atoms.push_back(a);
}

Atom getAtom(unsigned int i)
{
return atoms.at(i);
}

unsigned int size()
{
return atoms.size();
}

unsigned int atomMass()
{
unsigned int M = 0;

for (unsigned int i = 0; i < size(); i++)
	M += getAtom(i).atomMass();
	
return M;
}

char code()
{
	return nameToCode(name);
}

static void init()
{
names[0] = "ALA"; codes[0] = 'A';
names[1] = "ARG"; codes[1] = 'R';
names[2] = "ASN"; codes[2] = 'N';
names[3] = "ASP"; codes[3] = 'D';
names[4] = "CYS"; codes[4] = 'C';

names[5] = "GLU"; codes[5] = 'E';
names[6] = "GLN"; codes[6] = 'Q';
names[7] = "GLY"; codes[7] = 'G';
names[8] = "HIS"; codes[8] = 'H';
names[9] = "ILE"; codes[9] = 'I';

names[10] = "LEU"; codes[10] = 'L';
names[11] = "LYS"; codes[11] = 'K';
names[12] = "MET"; codes[12] = 'M';
names[13] = "PHE"; codes[13] = 'F';
names[14] = "PRO"; codes[14] = 'P';

names[15] = "SER"; codes[15] = 'S';
names[16] = "THR"; codes[16] = 'T';
names[17] = "TRP"; codes[17] = 'W';
names[18] = "TYR"; codes[18] = 'Y';
names[19] = "VAL"; codes[19] = 'V';

names[20] = "MSE"; codes[20] = 'M';
names[21] = "CME"; codes[21] = 'C';
names[22] = "HYP"; codes[22] = 'P';
names[23] = "LLP"; codes[23] = 'K';
names[24] = "CSB"; codes[24] = 'C';
names[25] = "CYG"; codes[25] = 'C';
names[26] = "CSE"; codes[26] = 'C';
names[27] = "OCS"; codes[27] = 'C';
names[28] = "TPO"; codes[28] = 'T';
names[29] = "PTR"; codes[29] = 'Y';

initialized = true;
}

static char nameToCode(string name)
{
	if (!initialized) init();

	for (int i = 0; i < RNUM; i++)
	{
		if (name == names[i]) return codes[i];
	}

	return '.';
}

static string codeToName(char code)
{
	if (!initialized) init();

	for (int i = 0; i < RNUM; i++)
	{
		if (code == codes[i]) return names[i];
	}

	return string();
}

};

bool Residue::initialized = false;
string Residue::names[RNUM];
char Residue::codes[RNUM];

class Molecule
{
public:
vector<Residue> res;

Molecule()
{

}

double diameter()
{
double D = 0;
for (unsigned int i = 0; i < size() - 1; i++)
for (unsigned int j = i + 1; j < size(); j++)
{
	Residue& R1 = getResidue(i);
	Residue& R2 = getResidue(j);

	if (R1.hasMainCarbon() && R2.hasMainCarbon())
	{
		Atom C1 = R1.getMainCarbon();
		Atom C2 = R2.getMainCarbon();
		D = max(D, C1.distance(C2));
	}
}
return D;
}

void addResidue(Residue r)
{
res.push_back(r);
}

Residue& lastResidue()
{
return res.back();
}

Residue& getResidue(unsigned int i)
{
return res.at(i);
}

unsigned int numAtoms()
{
unsigned int N = 0;

for (unsigned int i = 0; i < size(); i++)
	N += getResidue(i).size();
	
return N;
}

unsigned int atomMass()
{
unsigned int M = 0;

for (unsigned int i = 0; i < size(); i++)
	M += getResidue(i).atomMass();
	
return M;
}

unsigned int size()
{
return res.size();
}

void addMolecule(Molecule m)
{
for (unsigned int i = 0; i < m.size(); i++)
	addResidue(m.getResidue(i));
}

vector<double> principalMoments()
{
Matrix3f I;
double I_11 = 0;
double I_22 = 0;
double I_33 = 0;
double I_12 = 0;
double I_13 = 0;
double I_23 = 0;

vector<double> Mc = massCenter();

for (unsigned int i = 0; i < size(); i++)
{
	Residue& res = getResidue(i);
	for (unsigned int j = 0; j < res.size(); j++)
	{
		Atom a = res.getAtom(j);
		
		double m = a.atomMass();
		double x = a.x - Mc[0];
		double y = a.y - Mc[1];
		double z = a.z - Mc[2];
		
		I_11 += m * (y*y + z*z);
		I_22 += m * (x*x + z*z);
		I_33 += m * (x*x + y*y);
		I_12 -= m * x * y;
		I_13 -= m * x * z;
		I_23 -= m * y * z;
	}
}

I << 
I_11, I_12, I_13,
I_12, I_22, I_23,
I_13, I_23, I_33;

SelfAdjointEigenSolver<Matrix3f> eigensolver(I);
Vector3f V = eigensolver.eigenvalues();

vector<double> principalI;
principalI.push_back(V(0));
principalI.push_back(V(1));
principalI.push_back(V(2));
return principalI;
}

vector<double> massCenter()
{
double totalMass = 0;
vector<double> Mc(3, 0.0);
for (unsigned int i = 0; i < size(); i++)
{
	Residue& res = getResidue(i);
	for (unsigned int j = 0; j < res.size(); j++)
	{
		Atom a = res.getAtom(j);
		
		double m = a.atomMass();
		Mc[0] += m * a.x;
		Mc[1] += m * a.y;
		Mc[2] += m * a.z;
		
		totalMass += m;
	}
}

Mc[0] /= totalMass;
Mc[1] /= totalMass;
Mc[2] /= totalMass;

return Mc;
}

double asym3()
{
vector<double> I = principalMoments();
sort(I.rbegin(), I.rend());

return (I[0] - 0.5 * (I[1] + I[2])) / I[0];
}

double asym2()
{
vector<double> I = principalMoments();
sort(I.rbegin(), I.rend());

return (I[0] - I[2]) / I[0];
}

void writeAtomPDB
(FILE* pdb, unsigned int resNum, unsigned int atomNum, int serial)
{
Residue& r = getResidue(resNum);
Atom a = r.getAtom(atomNum);

fprintf(pdb, "%-6s%5d  %-3s%c%-3s ", 
"ATOM", serial, a.name.c_str(), ' ', r.name.c_str());

fprintf(pdb, "%c%4d    %8.3f%8.3f%8.3f", 
'A', resNum + 1, a.x, a.y, a.z);

fprintf(pdb, "%6.2f%6.2f%12s%-2s\n", 
a.occupancy, a.tempFactor, a.element.c_str(), " ");

fflush(pdb);
}

bool readXYZ(string fname)
{
	return readXYZ(fname.c_str());
}

bool readXYZ(const char* fname)
{
ifstream xyz(fname);
if (!xyz.is_open()) return false;

char str[82];

// Параметры записей
string name;
string resName;

double x = 0, y = 0, z = 0;
double tempFactor = 10;
string element, charge("");

while (xyz.getline(str, 82))
{
	string s(str);

	if (s.compare(0, 4, "RES:") == 0)
	{
	// Начало аминокислоты
	resName = s.substr(4, 3);
	addResidue(Residue(resName));
	}

	else if (s.compare(0, 1, " ") == 0)
	{
		// Атом аминокислоты
		name = s.substr(1, 3);
		element = s.substr(5, 1);
		x = atof(s.c_str() + 6);
		y = atof(s.c_str() + 14);
		z = atof(s.c_str() + 22);
		tempFactor = atof(s.c_str() + 31);

		Residue& res = lastResidue();
		res.addAtom(Atom(name, element, x, y, z, tempFactor));
	}
}
xyz.close();

return true;
}

bool readSEQ(string fname)
{
	return readSEQ(fname.c_str());
}

bool readSEQ(const char* fname)
{
	ifstream seq(fname);
	if (!seq.is_open()) return false;
	char str[82];

	while (seq.getline(str, 82))
	{
		string s(str);
		if (s.compare(0, 4, "") == 0)
		{
			// Начало строки последовательности
			for (int i = 4; i < 55; i++)
			{
				char code = str[i];
				string resName = Residue::codeToName(code);
				if (resName.length() > 0)
				{
					// Код верный
					addResidue(Residue(resName));
				}
			}
		}
	}
	seq.close();
	
	return true;
}

static bool readDomain(string fname, vector<Molecule>& domainParts)
{
	ifstream seq(fname.c_str());
	if (!seq.is_open()) return false;
	
	char str[82];

	domainParts.push_back(Molecule());

	while (seq.getline(str, 82))
	{
		string s(str);
		if (s.compare(0, 4, "    ") == 0)
		{
			// Начало строки последовательности
			for (int i = 4; i < 54; i++)
			{
				char code = str[i];
				string resName = Residue::codeToName(code);
				if (resName.length() > 0)
				{
					// Код верный
					domainParts.back().addResidue(Residue(resName));
				}
				else if (domainParts.back().size() != 0) {
					domainParts.push_back(Molecule());
				}
			}
		}
	}
	if (domainParts.back().size() == 0) domainParts.pop_back();
	seq.close();
	
	return true;
}

bool writePDB(string fname)
{
return writePDB(fname.c_str());
}

bool writePDB(const char* fname)
{
FILE* pdb = fopen(fname, "w");
if (pdb == NULL) return false;

int serial = 0;

for (unsigned int i = 0; i < size(); i++)
{
	Residue& res = getResidue(i);

	for (unsigned int j = 0; j < res.size(); j++)
	{
		writeAtomPDB(pdb, i, j, ++serial);
	}
}
fclose(pdb);

return true;
}

bool writeFASTA(const char* fname, const char* metainfo)
{
FILE* sqc = fopen(fname, "a");
if (sqc == NULL) return false;

int charsInLine = 0;
fprintf(sqc, ">%s\n", metainfo);

for (unsigned int i = 0; i < size(); i++)
{
	Residue& res = getResidue(i);

	char c = res.code();
	if (c != '.')
	{
		fprintf(sqc, "%c", c);
		if (++charsInLine == 80)
		{
			charsInLine = 0;
			fprintf(sqc, "\n");
		}
	}
}
fprintf(sqc, "\n");
fclose(sqc);

return true;
}

bool writeStat(const char* fname, const char* name)
{
FILE* out = fopen(fname, "a");
if (out == NULL) return false;

vector<double> I = principalMoments();

fprintf(out, "%s %8.3f %8d %8d %8d %8.1f %8.1f %8.1f\n", name, 
	diameter(), size(), numAtoms(), atomMass(), I[0], I[1], I[2]);

fclose(out);

return true;
}

string stringSequence()
{
string seq;
for (unsigned int i = 0; i < size(); i++)
	seq.append(1, getResidue(i).code());

return seq;
}

void fillDomain(Molecule& result, unsigned int start, unsigned int end)
{
for (unsigned int i = start; i < end; i++)
	result.addResidue(getResidue(i));
}

Molecule extractDomain(Molecule& domain)
{
string molSeq = stringSequence();
string domSeq = domain.stringSequence();

unsigned int start = molSeq.find(domSeq);
unsigned int end = start + domSeq.length();

Molecule result;

if (start < 100000) fillDomain(result, start, end);
else {
	cout << endl << "Error while extracting domain" << endl;
	cout << ">" << molSeq << endl;
	cout << ">" << domSeq << endl;
}

return result;
}

};

class MoleculeParams
{
public:
static const int DIAMETER = 0;
static const int MASS = 1;
static const int NUM_ATOMS = 2;
static const int NUM_RESIDUES = 3;
static const int ASYM_3 = 4;
static const int ASYM_2 = 5;

static const char* names[];
};

const char* MoleculeParams::names[] = {
"Diameter",
"Mass",
"Num_atoms",
"Num_residues",
"Asym_3",
"Asym_2"
};

#endif

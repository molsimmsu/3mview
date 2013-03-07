#ifndef DOMAINS_DB_H
#define DOMAINS_DB_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cfloat>
#include "Molecule.h"
using namespace std;

class Domain
{
private:

string name;
float diam;
int res;
int atoms;
int mass;
double I1, I2, I3;

public:

Domain(string name, float diameter, int residues, int atoms, int mass,
	double I1, double I2, double I3):
	name(name),
	diam(diameter),
	res(residues),
	atoms(atoms),
	mass(mass),
	I1(I1),
	I2(I2),
	I3(I3) { }
	
double get(int paramName)
{
	switch(paramName)
	{
		case MoleculeParams::DIAMETER: return diam;
		case MoleculeParams::MASS: return mass;
		case MoleculeParams::NUM_ATOMS: return atoms;
		case MoleculeParams::NUM_RESIDUES: return res;
		case MoleculeParams::ASYM_3: return asym3();
		case MoleculeParams::ASYM_2: return asym2();
		default: return 0;
	}
}
	
double diameter()
{
	return diam;
}

string domainName()
{
	return name;
}

string moleculeName()
{
	return name.substr(0, 5);
}

string domainPath()
{
	return string("DomainsDB/domains/") + domainName() + ".pdb";
}

string moleculePath()
{
	return string("DomainsDB/molecules/") + moleculeName() + ".pdb";
}

double asym3()
{
vector<double> I;
I.push_back(I1);
I.push_back(I2);
I.push_back(I3);
sort(I.rbegin(), I.rend());

return (I[0] - 0.5 * (I[1] + I[2])) / I[0];
}

double asym2()
{
vector<double> I;
I.push_back(I1);
I.push_back(I2);
I.push_back(I3);
sort(I.rbegin(), I.rend());

return (I[0] - I[2]) / I[0];
}

};

class DomainsDBConstraint
{
public:

double dmin, dmax, asym3Max, asym3Min, asym2Max, asym2Min;

DomainsDBConstraint()
{
	dmin = 0;
	dmax = DBL_MAX;
	asym3Min = 0;
	asym3Max = DBL_MAX;
	asym2Min = 0;
	asym2Max = DBL_MAX;
}

bool satisfies(Domain d)
{
	if (d.diameter() < dmin) return false;
	if (d.diameter() > dmax) return false;
	if (d.asym3() < asym3Min) return false;
	if (d.asym3() > asym3Max) return false;
	if (d.asym2() < asym2Min) return false;
	if (d.asym2() > asym2Max) return false;
	
	return true;
}

bool satisfies(Molecule m)
{
	if (m.diameter() < dmin) return false;
	if (m.diameter() > dmax) return false;
	if (m.asym3() < asym3Min) return false;
	if (m.asym3() > asym3Max) return false;
	if (m.asym2() < asym2Min) return false;
	if (m.asym2() > asym2Max) return false;
	
	return true;
}

};

class DomainsDB
{
private:

vector<Domain> domains;

void readDomains()
{
	char name[8];
	float diameter;
	int residues;
	int atoms;
	int mass;
	double I1, I2, I3;

	FILE* f = fopen("DomainsDB/stat", "r");
	
	if (f == NULL) cout << "Error while reading file" << endl;
	
	while (!feof(f))
	{
		fscanf(f, "%s %f %d %d %d %lf %lf %lf\n",
			name, &diameter, &residues, &atoms, &mass, &I1, &I2, &I3);
		
		domains.push_back(
			Domain(string(name), diameter, residues, atoms, mass, I1, I2, I3)
		);
	}
	
	fclose(f);
}

public:

DomainsDB()
{
	readDomains();
}

// Количество доменов в БД
unsigned int size()
{
	return domains.size();
}

void verboseSize(ostream& out)
{
	out << "DomainsDB size: " << size() << endl;
}

// i-й домен
Domain at(unsigned int i)
{
	return domains.at(i);
}

vector<Domain> selectDomains(DomainsDBConstraint c)
{
	vector<Domain> selected;
	
	for (unsigned int i = 0; i < size(); i++)
	{
		Domain d = at(i);
		if (c.satisfies(d)) selected.push_back(d);
	}
	
	return selected;
}

static vector< vector<string> > domainsToMolecules(vector<Domain> doms)
{
	vector< vector<string> > vec;
	vector<string> v;
	for (unsigned int i = 0; i < doms.size(); i++)
	{
		if (i == 0 || doms[i].moleculeName() != doms[i-1].moleculeName())
		{
			v.clear();
			v.push_back(doms[i].moleculePath());
		}
		
		v.push_back(doms[i].domainPath());
		
		if (i == doms.size() - 1 || 
			doms[i].moleculeName() != doms[i+1].moleculeName())
		{
			vec.push_back(v);
		}
	}
	return vec;
}

};

#endif

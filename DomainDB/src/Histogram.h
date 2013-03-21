#ifndef MOLSIM_HISTOGRAM_H
#define MOLSIM_HISTOGRAM_H

#define __int64 long long

#include "DomainsDB.h"
#include <plplot/plConfig.h>
#include <plplot/plplot.h>

class Histogram
{
private:
	vector<Domain> v;

public:

Histogram(vector<Domain>& v)
{
	this->v = v;
}

void build(int paramID)
{
	double data[v.size()];
	
	for (unsigned int i = 0; i < v.size(); i++)
		data[i] = v[i].get(paramID);
	
	write(MoleculeParams::names[paramID], data);
}

void write(const char* name, double* data)
{
	/*
	double* max = max_element(data, data + v.size());;
	string xAxis = string("#fr") + name;
	string title = string("#fr") + name + " distribution";

	string strName("DomainsDB/histograms/");
	strName += name;
	strName += ".png";
	
	// plplot required
	plsdev("pngcairo");
	plsfnam(strName.c_str());
	plsetopt("bg", "FFFFFF");
	plinit();

	plscol0(1, 0, 0, 0);
	plcol0( 1 );

	plhist( v.size(), data, 0, *max, 40, 0 );
	pllab( xAxis.c_str(), "#frNumDomains",
		      title.c_str() );

	plend();
	*/
}


};

#endif

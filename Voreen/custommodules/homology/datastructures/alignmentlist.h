#ifndef ALIGNMENT_H
#define ALIGNMENT_H

#include <algorithm>
#include <string>
#include <iostream>
#include <cstdio>
using namespace std;

class Alignment
{
public:

	string name;
	double pident;
	int qstart, qend;
	
	Alignment(const char* name, double pident, int qstart, int qend)
	{
		this->name = name;
		this->pident = pident;
		this->qstart = qstart;
		this->qend = qend;
	}

	void write(ostream& out) const
	{
		out << name << " " << pident << " " << qstart << " " << qend << endl;
	}

	bool operator < (const Alignment& A) const
	{
		if (pident < A.pident) return true;
		return false;
	}

};

class AlignmentList
{
private:
	vector<Alignment> A;
	string sourceSeq;

public:

	unsigned int sourceLength()
	{
		return sourceSeq.size();
	}

		unsigned int size()
		{
			return A.size();
		}

		Alignment at(unsigned int i)
		{
			return A.at(i);
		}

	void write(ostream& out)
	{
		for (unsigned int i = 0; i < A.size(); i++)
		{
			A.at(i).write(out);
		}
	}
	
	bool operator != (const AlignmentList& A) const
	{
		return false;
	}

	void sortByIdentityPercent()
	{
		sort(A.rbegin(), A.rend());
	}

	void findHomologousBLAST(string seq, string dir = ".")
	{
		sourceSeq = seq;
		std::string cmd;
		cmd += dir + "/bdf --pipe " + seq;
		cmd += " | " + dir + "/blastp";
		cmd += " -outfmt \'6 sseqid pident qstart qend\'";
		cmd += " -db " + dir + "/DomainsDB/domains";
		cmd += " -num_alignments 50";
		cmd += " -evalue 10";
		
		FILE* pipe = popen(cmd.c_str(), "r");

		while (!feof(pipe))
		{
			char sseqid[8]; float pident;
			int qstart, qend;
			fscanf(pipe, "%s %f %d %d", sseqid, &pident, &qstart, &qend);
			A.push_back(Alignment(sseqid, pident, qstart, qend));
		}
		
		A.pop_back();
	
		pclose(pipe);
	}

};

#endif

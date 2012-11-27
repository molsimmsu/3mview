#include "StrideReader.h"

StrideReader::StrideReader(const char* pdbPath)
{
	// Запускаем STRIDE
	string cmd("./stride ");
	cmd += pdbPath;
	FILE* pipe = popen(cmd.c_str(), "r");

	// Построчно читаем вывод
	char buf[100];
	while (!feof(pipe))
	{
		buf[0] = 0;
		fgets(buf, 100, pipe);
		if (strncmp(buf, "ASG", 3) == 0) parseLine(buf);
		if (strncmp(buf, "Error", 5) == 0)
		{
			// TODO Обрабатываем ошибки
			break;
		}
	}

	pclose(pipe);
}

void StrideReader::parseLine(const char* str)
{
	char structure = str[24]; // Тип вторичной структуры
	unsigned int chainNum = str[9] - 'A';
	if (chainNum >= numChains()) data.push_back(vector<char>());
	data[chainNum].push_back(structure);
}

unsigned int StrideReader::numChains()
{
	return data.size();
}

unsigned int StrideReader::numResidues(unsigned int chainNum)
{
	return data[chainNum].size();
}

char StrideReader::secondaryStructure(unsigned int chainNum, unsigned int residueNum)
{
	return data[chainNum][residueNum];
}

#ifndef NAMEDISPATCHER_H
#define NAMEDISPATCHER_H

#include "RGBColor.h"
#include "DataObject.h"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class ObjectDispatcher
{
public:
    ObjectDispatcher();
    static string setName(DataObject* obj);
    static RGBColor setColor(DataObject* obj);
    static int prefixCounter(string prefix);

private:
    static vector<string> prefixes;
    static vector<int> counters;
};

#endif // NAMEDISPATCHER_H

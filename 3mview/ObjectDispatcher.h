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
    string setName(DataObject* obj);
    RGBColor setColor(DataObject* obj);
    int prefixCounter(string prefix);

private:
    vector<string> prefixes;
    vector<int> counters;
};

#endif // NAMEDISPATCHER_H

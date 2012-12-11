#include "ObjectDispatcher.h"

vector<string> ObjectDispatcher::prefixes;
vector<int> ObjectDispatcher::counters;

ObjectDispatcher::ObjectDispatcher()
{
}

string ObjectDispatcher::setName(DataObject *obj)
{
    string prefix = obj->typeName();
    stringstream ss;
    ss << prefixCounter(prefix);
    string name = prefix + ss.str();
    obj->setName(name);
    return name;
}

RGBColor ObjectDispatcher::setColor(DataObject *obj)
{
    RGBColor color = RGBColor(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
    obj->setColor(color);
    return color;
}

int ObjectDispatcher::prefixCounter(string prefix)
{
    vector<string>::iterator it = find(prefixes.begin(), prefixes.end(), prefix);

    if (it == prefixes.end())
    {
        prefixes.push_back(prefix);
        counters.push_back(1);
        return 1;
    }

    unsigned int i = it - prefixes.begin();
    return ++counters[i];
}

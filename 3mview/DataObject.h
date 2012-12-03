#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include "RGBColor.h"
#include <string>
using namespace std;

// Базовый класс для объектов молекул, карт и прочих
class DataObject
{
public:
    DataObject();

    virtual string typeName() = 0;

    void setName(string name) { _name = name; }
    string name() { return _name; }

    void setColor(RGBColor color) { _color = color; }
    RGBColor color() { return _color; }

private:
    string _name;
    RGBColor _color;
};

#endif // DATAOBJECT_H

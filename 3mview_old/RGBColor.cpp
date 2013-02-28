#include "RGBColor.h"

RGBColor::RGBColor():
_r(100), _g(100), _b(100)
{}

RGBColor::RGBColor(double r, double g, double b):
_r(r), _g(g), _b(b)
{}

double RGBColor::r()
{
    return _r;
}

double RGBColor::g()
{
    return _g;
}

double RGBColor::b()
{
    return _b;
}

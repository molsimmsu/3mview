#include "RGBColor.h"

RGBColor::RGBColor():
_r(100), _g(100), _b(100)
{}

RGBColor::RGBColor(int r, int g, int b):
_r(r), _g(g), _b(b)
{}

int RGBColor::r()
{
    return _r;
}

int RGBColor::g()
{
    return _g;
}

int RGBColor::b()
{
    return _b;
}

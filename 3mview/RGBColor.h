#ifndef COLOR_H
#define COLOR_H

class RGBColor
{
public:
    RGBColor();
    RGBColor(double r, double g, double b);
    double r();
    double g();
    double b();

private:
    double _r, _g, _b;
};

#endif // COLOR_H

#ifndef COLOR_H
#define COLOR_H

class RGBColor
{
public:
    RGBColor();
    RGBColor(int r, int g, int b);
    int r();
    int g();
    int b();

private:
    int _r, _g, _b;
};

#endif // COLOR_H

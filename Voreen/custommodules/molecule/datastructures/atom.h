#ifndef VRN_ATOM_H
#define VRN_ATOM_H

#include <string>
#include <iostream>

class Atom
{
private:
std::string type_;
double x_, y_, z_;
double occupancy_, tempFactor_;
size_t atomicNumber_;

public:

Atom(std::string type, 
	double x, double y, double z, size_t atomicNumber, double tempFactor = 0.0, double occupancy = 1.0)
{
type_ = type;
x_ = x;
y_ = y;
z_ = z;
atomicNumber_ = atomicNumber;
tempFactor_ = tempFactor;
occupancy_ = occupancy;
}

size_t atomicNumber() const { return atomicNumber_; }
std::string type() const { return type_; }
double x() const { return x_; }
double y() const { return y_; }
double z() const { return z_; }

double distance(Atom& A)
{
double dx = x_ - A.x();
double dy = y_ - A.y();
double dz = z_ - A.z();
return sqrt(dx*dx + dy*dy + dz*dz);
}

unsigned int atomMass()
{
if (type_ == "C") return 12;
if (type_ == "N") return 14;
if (type_ == "O") return 16;
if (type_ == "P") return 31;
if (type_ == "S") return 32;
if (type_ == "U") return 238;

std::cout << "Atomic mas not defined: " << type_ << std::endl;

return 0;
}

};

#endif // VRN_ATOM_H

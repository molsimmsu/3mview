#include "molmolalign.h"

#define    SCALE          30
#define    SOLVE_ITER     52
#define    PI_2           1.57079632679
#define    MAX_SIZE        1.5    // range of the first harmonic


const std::string MolMolAlign::loggerCat_("3mview.MolMolAlign");

MolMolAlign :: MolMolAlign()
  : Processor(),
    tobealigned_("tobealigned", "Molecule to reorientate", Processor::INVALID_PROGRAM),
    align_("align", "Align", Processor::INVALID_PROGRAM),
    molinport1_(Port::INPORT,   "molecule1", "Molecule 1"),
    molinport2_(Port::INPORT,   "molecule2", "Molecule 2"),
    outport_(Port::OUTPORT,   "molecule3", "Molecule")
{
    tobealigned_.addOption("1to2", "1 to 2");
    tobealigned_.addOption("2to1", "2 to 1");
    tobealigned_.addOption("1toOrigin", "1 to Origin");
    tobealigned_.addOption("2toOrigin", "2 to Origin");
    addProperty(tobealigned_);
    addProperty(align_);

    addPort(molinport1_);
    addPort(molinport2_);
    addPort(outport_);
    
    align_.onClick(CallMemberAction<MolMolAlign>(this, &MolMolAlign::align));
}

void MolMolAlign :: align()
{
	if (tobealigned_.isSelected("1to2") || tobealigned_.isSelected("2to1"))
	{
		const Molecule* firstMol;
		const Molecule* secondMol;
		
		if (tobealigned_.isSelected("1to2")) {
		    firstMol  = molinport1_.getData();
		    secondMol = molinport2_.getData();
		}
		if (tobealigned_.isSelected("2to1")) {
		    firstMol  = molinport2_.getData();
		    secondMol = molinport1_.getData();
		}

	    Molecule* outMol = firstMol->clone();

		tgt::mat4 fit1;
		tgt::mat4 fit2;
		tgt::mat4 invertedfit2;
		tgt::mat4 wrld1;
		tgt::mat4 invertedwrld1;

	     LINFO("Getting transformation matrix for molecule #1..");
		fit1 = GetTransformation(firstMol);	

          LINFO("Getting transformation matrix for molecole #2..");
		fit2 = GetTransformation(secondMol);
		fit2.invert(invertedfit2);

		wrld1 =  firstMol ->getTransformationMatrix();
		wrld1.invert(invertedwrld1);

		const tgt::mat4  _temp = invertedwrld1*invertedfit2*fit1*wrld1;		
		outMol->transform(_temp);
		outport_.setData(outMol);
	}

}

tgt::mat4 MolMolAlign :: GetTransformation(const Molecule* mol)
{
     O[0] = 0;
	O[1] = 0;
	O[2] = 0;
     total_weight = 0;

	entries = (mol->getOBMol()).NumAtoms();
	coords = new float[4*entries];
	for (int i=0; i<entries; ++i)
	{	
		coords[4*i]   = (mol->getOBMol()).GetAtomById(i)->x();
		coords[4*i+1] = (mol->getOBMol()).GetAtomById(i)->y();
		coords[4*i+2] = (mol->getOBMol()).GetAtomById(i)->z();
		coords[4*i+3] = (mol->getOBMol()).GetAtomById(i)->GetFormalCharge(); // TODO !!

		O[0] += coords[4*i]   * coords[4*i+3];
		O[1] += coords[4*i+1] * coords[4*i+3];
		O[2] += coords[4*i+2] * coords[4*i+3];
		total_weight += coords[4*i+3];
	}

	O[0] /= total_weight;
	O[1] /= total_weight;
	O[2] /= total_weight;

	for (int i=0; i<entries; ++i)
	{	
		coords[4*i]   -= O[0];
		coords[4*i+1] -= O[1];
		coords[4*i+2] -= O[2];
	}

	FindAxes();
	delete[] coords;

	tgt::mat4 out_data;
	
	out_data.t00 = Ox[0];
	out_data.t10 = Ox[1];
	out_data.t20 = Ox[2];
	out_data.t30 = 0;
		
	out_data.t01 = Oy[0];
	out_data.t11 = Oy[1];
	out_data.t21 = Oy[2];
	out_data.t31 = 0;

	out_data.t02 = Oz[0];
	out_data.t12 = Oz[1];
	out_data.t22 = Oz[2];
	out_data.t32 = 0;
	
	out_data.t03 = 0;
	out_data.t13 = 0;
	out_data.t23 = 0;
	out_data.t33 = 1;
	
	
	tgt::mat4 out_data1;
	
	out_data1.t00 = 1;
	out_data1.t10 = 0;
	out_data1.t20 = 0;
	out_data1.t30 = 0;
		
	out_data1.t01 = 0;
	out_data1.t11 = 1;
	out_data1.t21 = 0;
	out_data1.t31 = 0;

	out_data1.t02 = 0;
	out_data1.t12 = 0;
	out_data1.t22 = 1;
	out_data1.t32 = 0;
	
	out_data1.t03 = -O[0];
	out_data1.t13 = -O[1];
	out_data1.t23 = -O[2];
	out_data1.t33 =  1;	

    tgt::mat4 out_data2 = out_data*out_data1;
    return out_data2;
}

double MolMolAlign :: CalculateMoment(int degX, int degY, int degZ)
{
	double res = 0;
	double temp;
	for (int i=0; i<entries; ++i)
	{
		temp = coords[4*i+3];
		for (int j = 0; j < degX; ++j)
		{
			temp *= coords[4*i]/SCALE;
		}
		
		for (int j = 0; j < degY; ++j)
		{
			temp *= coords[4*i+1]/SCALE;
		}
		
		for (int j = 0; j < degZ; ++j)
		{
			temp *= coords[4*i+2]/SCALE;
		}

		res += temp/total_weight;
	}
	return res;
}

double MolMolAlign :: CalculateFourrier(int degX, int degY, int degZ)
{
	double res = 0;
	double temp;
	for (int i=0; i<entries; ++i)
	{
		temp = coords[4*i+3];
		if (degX<0) temp *= cos(degX*coords[4*i]  /SCALE/MAX_SIZE*PI_2);
		if (degX>0) temp *= sin(degX*coords[4*i]  /SCALE/MAX_SIZE*PI_2);
		if (degY<0) temp *= cos(degY*coords[4*i+1]/SCALE/MAX_SIZE*PI_2);
		if (degY>0) temp *= sin(degY*coords[4*i+1]/SCALE/MAX_SIZE*PI_2);
		if (degZ<0) temp *= cos(degZ*coords[4*i+2]/SCALE/MAX_SIZE*PI_2);
		if (degZ>0) temp *= sin(degZ*coords[4*i+2]/SCALE/MAX_SIZE*PI_2);
		res += temp;
	}
	return res;
}

void MolMolAlign :: FindAxes()
{
	double disc;
	double I[3][3];
	double eigens[3];

	double V1[3];
	double V2[3];
	double a = -1;
	double b = 1;
	
	I[0][0] = CalculateMoment(2, 0, 0);
	I[1][1] = CalculateMoment(0, 2, 0);
	I[2][2] = CalculateMoment(0, 0, 2);
	I[0][1] = CalculateMoment(1, 1, 0);
	I[0][2] = CalculateMoment(1, 0, 1);
	I[1][2] = CalculateMoment(0, 1, 1);
	I[1][0] = I[0][1];
	I[2][0] = I[0][2];
	I[2][1] = I[1][2];


	polynom[0] =  I[0][0]*I[1][1]*I[2][2]+2*I[0][1]*I[0][2]*I[1][2]-I[0][2]*I[0][2]*I[1][1]-I[2][1]*I[2][1]*I[0][0]-I[0][1]*I[0][1]*I[2][2];
	polynom[1] = -(I[1][1]*I[2][2]+I[2][2]*I[0][0]+I[0][0]*I[1][1]-I[0][2]*I[0][2]-I[1][2]*I[1][2]-I[0][1]*I[0][1]);
	polynom[2] = I[0][0] + I[1][1] + I[2][2];
	polynom[3] = -1;

	while (PolynomVal(a)<0) {a-=1;}
	while (PolynomVal(b)>0) {b+=1;}
	for (int i=0; i<SOLVE_ITER; ++i)
	{
		if (PolynomVal((b+a)/2) > 0)
		{
			a = (b+a)/2;
		}
		else 
		{
			b = (b+a)/2;
		}
	}
	eigens[0] = a;
	
	polynom[0] = polynom[1]+a*polynom[2]+a*a*polynom[3];
	polynom[1] = polynom[2]+a*polynom[3];
	polynom[2] = polynom[3];
	polynom[3] = 0;
	

	disc = polynom[1]*polynom[1] - 4*polynom[0]*polynom[2];
	if (disc < 0) 
	{
  		LINFO("Negative eigenvalues!");
		return;
	}
	else
	{
		eigens[1] = -(polynom[1]-sqrt(disc))/(2*polynom[2]);
		eigens[2] = -(polynom[1]+sqrt(disc))/(2*polynom[2]);
	}
	
	for (int i=0; i<3; ++i)
	{
		for (int j=i; j<3; ++j)	
		{
			if (eigens[i] > eigens[j])
			{
				disc = eigens[i];
				eigens[i] = eigens[j];
				eigens[j] = disc;
			}
		}
	}	

	V1[0] = I[0][0] - eigens[0]; 
	V1[1] = I[0][1];
	V1[2] = I[0][2];

	V2[0] = I[1][0]; 
	V2[1] = I[1][1] - eigens[0];
	V2[2] = I[1][2];

	Ox[0] = V1[1]*V2[2] - V1[2]*V2[1];	
	Ox[1] = V1[2]*V2[0] - V1[0]*V2[2];	
	Ox[2] = V1[0]*V2[1] - V1[1]*V2[0];
	
	double len = sqrt(Ox[0]*Ox[0]+Ox[1]*Ox[1]+Ox[2]*Ox[2]);
	Ox[0] /= len;
	Ox[1] /= len;
	Ox[2] /= len;


	V1[0] = I[1][0];
	V1[1] = I[1][1] - eigens[1]; 
	V1[2] = I[1][2];

	V2[0] = I[2][0]; 
	V2[1] = I[2][1];
	V2[2] = I[2][2] - eigens[1];

	Oy[0] = V1[1]*V2[2] - V1[2]*V2[1];	
	Oy[1] = V1[2]*V2[0] - V1[0]*V2[2];	
	Oy[2] = V1[0]*V2[1] - V1[1]*V2[0];

	len = sqrt(Oy[0]*Oy[0]+Oy[1]*Oy[1]+Oy[2]*Oy[2]);
	Oy[0] /= len;
	Oy[1] /= len;
	Oy[2] /= len;

	V1[0] = I[0][0] - eigens[2]; 
	V1[1] = I[0][1];
	V1[2] = I[0][2];

	V2[0] = I[2][0]; 
	V2[1] = I[2][1];
	V2[2] = I[2][2] - eigens[2];

	Oz[0] = V1[1]*V2[2] - V1[2]*V2[1];	
	Oz[1] = V1[2]*V2[0] - V1[0]*V2[2];	
	Oz[2] = V1[0]*V2[1] - V1[1]*V2[0];

	len = sqrt(Oz[0]*Oz[0]+Oz[1]*Oz[1]+Oz[2]*Oz[2]);
	Oz[0] /= len;
	Oz[1] /= len;
	Oz[2] /= len;

	disc =  Ox[0]*Oy[1]*Oz[2]+Ox[1]*Oy[2]*Oz[0]+Oy[0]*Oz[1]*Ox[2]-Ox[2]*Oz[0]*Oy[1]-Oz[1]*Oy[2]*Ox[0]-Ox[1]*Oy[0]*Oz[2];
	if (disc<0)
	{
		Oz[0] = -Oz[0];
		Oz[1] = -Oz[1];
		Oz[2] = -Oz[2];
	}
	
	if (CalculateMoment(3, 0, 0)<0)
	{
		Ox[0] = -Ox[0];
		Ox[1] = -Ox[1];
		Ox[2] = -Ox[2];

		Oy[0] = -Oy[0];
		Oy[1] = -Oy[1];
		Oy[2] = -Oy[2];
	}			
	if (CalculateMoment(0, 3, 0)<0)
	{
		Oz[0] = -Oz[0];
		Oz[1] = -Oz[1];
		Oz[2] = -Oz[2];

		Oy[0] = -Oy[0];
		Oy[1] = -Oy[1];
		Oy[2] = -Oy[2];
	}
}

double MolMolAlign :: PolynomVal(double x)
{
	return polynom[3]*x*x*x + polynom[2]*x*x + polynom[1]*x + polynom[0];
}

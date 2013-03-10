#include "alignbymoments.h"

#define    SCALE          30
#define    SOLVE_ITER  	 52
#define    PI_2           1.57079632679
#define    MAX_SIZE	      1.5    // range of the first harmonic


AlignByMoments :: AlignByMoments()
  : Processor(),
    sourceselection_("sourcetype", "Select Source", Processor::INVALID_PROGRAM),
    volinport_(Port::INPORT,   "volume",   "Electon density map"),
    molinport_(Port::INPORT,   "molecule", "Molecule structure"),
    outport_(Port::OUTPORT, "Matrix4double", "New coordinate system output")
{
    sourceselection_.addOption("mol", "Molecule[Atoms set]");
    sourceselection_.addOption("vol", "Volume  [Electrun density]");
    addProperty(sourceselection_);
    addPort(molinport_);
    addPort(volinport_);
    addPort(outport_);
}

void AlignByMoments :: process()
{
	O[0] = 0;
	O[1] = 0;
	O[2] = 0;
     total_weight = 0;
	if (sourceselection_.isSelected("mol"))
	{
		entries = (molinport_.getData()->getOBMol()).NumAtoms();
		coords = new double[4*entries];
		for (int i=0; i<entries; ++i)
		{	
			coords[4*i]   = (molinport_.getData()->getOBMol()).GetAtomById(i)->x();
			coords[4*i+1] = (molinport_.getData()->getOBMol()).GetAtomById(i)->y();
			coords[4*i+2] = (molinport_.getData()->getOBMol()).GetAtomById(i)->z();
			coords[4*i+3] = (molinport_.getData()->getOBMol()).GetAtomById(i)->GetFormalCharge(); // TODO !!

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

		PDBFindAxes();
		PDBFillOutport();
		delete[] coords;
	}
	if (sourceselection_.isSelected("vol"))
	{
	}
}

double AlignByMoments :: CalculateMoment(int degX, int degY, int degZ)
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

double AlignByMoments :: CalculateFourrier(int degX, int degY, int degZ)
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

void AlignByMoments :: PDBFillOutport()
{
		tgt::Matrix4<double>* out_data = new tgt::Matrix4<double>(); // Must be a pointer
		
		out_data->t00 = Ox[0];
		out_data->t10 = Ox[1];
		out_data->t20 = Ox[2];
		
		out_data->t01 = Oy[0];
		out_data->t11 = Oy[1];
		out_data->t21 = Oy[2];

		out_data->t02 = Oz[0];
		out_data->t12 = Oz[1];
		out_data->t22 = Oz[2];
	
		out_data->t03 = O[0];
		out_data->t13 = O[1];
		out_data->t23 = O[2];

		out_data->t30 = 0;
		out_data->t31 = 0;
		out_data->t32 = 0;
		out_data->t33 = 1;


		outport_.setData(out_data);
}

void AlignByMoments :: PDBFindAxes()
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
//		Eigenvalues are complex
		exit(8);
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

double AlignByMoments :: PolynomVal(double x)
{
	return polynom[3]*x*x*x + polynom[2]*x*x + polynom[1]*x + polynom[0];
}

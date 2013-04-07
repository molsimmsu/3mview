#include "pointcloud.h"
 
using namespace voreen;

void PointCloud :: SetOrientation(tgt::mat4 arg)
{
	double z;
     z = arg.elem[0]*(arg.elem[5]*arg.elem[10] - arg.elem[6]*arg.elem[9]) + arg.elem[1]*(arg.elem[6]*arg.elem[8] - arg.elem[4]*arg.elem[10]) + arg.elem[2]*(arg.elem[4]*arg.elem[9] - arg.elem[5]*arg.elem[8]);
	if (z>0)	orientation = 1;
	else      orientation = -1;
}
 
PointCloud :: PointCloud()
{
	have_points  = false;
	have_moments = false;
	total_weight = 0;
	max_value    = 0;
	min_value    = 0;
}

PointCloud :: ~PointCloud()
{
	if (have_points)
	{
		delete[] points;
		delete[] values;
	}
	if (have_moments)
	{
		delete[] moments;
	}
}


void PointCloud :: VolumeFill(const Volume* vol)
{
	VolumeFill(vol, 0);
}

void PointCloud :: VolumeFill(const Volume* vol, double min)
{

	const VolumeRAM* volRam = vol->getRepresentation<VolumeRAM>();
	RealWorldMapping rwm    = vol->getRealWorldMapping();
	SetOrientation(vol->getVoxelToWorldMatrix());

	tgt::svec3 dims   = vol->getDimensions();
	tgt::dvec4 pworld;
	double     valNorm;
	double     valRW;
	int        non_zero    = 0; 
	total_weight 	 = 0;

	entries_num =  dims.x*dims.y*dims.z;
	points      = new tgt::vec3[entries_num+1];
     values      = new double[entries_num+1];
	max_value   = 0;
	min_value   = min;

	for (int i=0; i<dims.x; ++i)
		for (int j=0; j<dims.y; ++j)
			for (int k=0; k<dims.z; ++k)
			{
				valNorm = volRam->getVoxelNormalizedLinear(tgt::dvec3(0.5+i, 0.5+j, 0.5+k));
				valRW = rwm.normalizedToRealWorld(valNorm);
				if (fabs(valRW)>min_value)
				{
					pworld = vol->getVoxelToWorldMatrix() * (tgt::dvec4(0.5+i, 0.5+j, 0.5+k, 1.0));	
					points[non_zero][0] = pworld.x;
					points[non_zero][1] = pworld.y;
					points[non_zero][2] = pworld.z;
					values[non_zero] = valRW;
					if (fabs(valRW)>max_value)
					{
						max_value = fabs(valRW);
					}
					total_weight += valRW;
					non_zero++;
				}
			}
	entries_num = non_zero;
	have_points = true;
}


void PointCloud :: MoleculeFill(const Molecule * mol)
{
	std :: cout << "Loading molecule...\n";
	entries_num = (mol->getOBMol()).NumAtoms();
	int non_zero 	  = 0; 
	int atomic_num	  = 0;

	points      = new tgt::vec3[entries_num+1];
     values      = new double[entries_num+1];
	total_weight 	  = 0;

	for (int i=0; i<entries_num; ++i)
	{	
		atomic_num = (mol->getOBMol()).GetAtomById(i)->GetAtomicNum();
		if (atomic_num == 6)
			{
				points[non_zero].x   = (mol->getOBMol()).GetAtomById(i)->x();
				points[non_zero].y   = (mol->getOBMol()).GetAtomById(i)->y();
				points[non_zero].z   = (mol->getOBMol()).GetAtomById(i)->z();
				values[non_zero] = atomic_num;
				total_weight += values[non_zero];
				non_zero++;
			}	
	}
	entries_num = non_zero;
	have_points = true;
	printf("Structure total weight : %lf\n", total_weight);
}


double PointCloud :: CalculateFourrier(int degX, int degY, int degZ)
{
	double res = 0;
	double temp;
	for (int i=0; i<entries_num; ++i)
	{
		temp = values[i];
		if (degX<0) temp *= cos(degX*points[i].x*PI_2/STRUCT_SIZE);
		if (degX>0) temp *= sin(degX*points[i].x*PI_2/STRUCT_SIZE);
		if (degY<0) temp *= cos(degY*points[i].y*PI_2/STRUCT_SIZE);
		if (degY>0) temp *= sin(degY*points[i].y*PI_2/STRUCT_SIZE);
		if (degZ<0) temp *= cos(degZ*points[i].z*PI_2/STRUCT_SIZE);
		if (degZ>0) temp *= sin(degZ*points[i].z*PI_2/STRUCT_SIZE);
		res += temp;
	}
	return res/total_weight;
}

double PointCloud :: CalculateMoment(int degX, int degY, int degZ)
{
	double res = 0;
	double temp;
	for (int i=0; i<entries_num; ++i)
	{
		temp = values[i];
		for (int j = 0; j < degX; ++j)
		{
			temp *= points[i].x/STRUCT_SIZE;
		}
		
		for (int j = 0; j < degY; ++j)
		{
			temp *= points[i].y/STRUCT_SIZE;
		}

		
		for (int j = 0; j < degZ; ++j)
		{
			temp *= points[i].z/STRUCT_SIZE;
		}
		res += temp;
	}
	return res/total_weight;
}

double PointCloud :: PolynomVal(double x)
{
	return polynom[3]*x*x*x + polynom[2]*x*x + polynom[1]*x + polynom[0];
}

tgt::Matrix4d PointCloud :: GetShift()
{

	std :: cout << "Calculating mass center... \n";
     O[0] = 0;
	O[1] = 0;
	O[2] = 0;

     total_weight = 0;

	for (int i=0; i<entries_num; ++i)
	{	
		O[0] += points[i].x * values[i];
		O[1] += points[i].y * values[i];
		O[2] += points[i].z * values[i];
		total_weight += values[i];
    }

	O[0] /= total_weight;
	O[1] /= total_weight;
	O[2] /= total_weight;
	Centrify();

	printf("Mass center: %.2lf, %.2lf, %.2lf\n", O[0], O[1], O[2]);

	tgt::Matrix4d out_data(1,     0,     0, -O[0],
					   0,     1,     0, -O[1],
			 		   0,     0,     1, -O[2],
					   0,     0,     0,    1);
	return out_data;
}

void PointCloud :: Centrify()
{
	std :: cout << "Centrifying... \n";
	for (int i=0; i<entries_num; ++i)
	{	
		points[i].x -= O[0];
		points[i].y -= O[1];
		points[i].z -= O[2];
	}
}

tgt::Matrix4d PointCloud :: GetAxes()
{
	std :: cout << "Getting axes... \n";
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
	for (int i=0; i<50; ++i)
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
		return tgt::Matrix4d::identity;
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
			if (fabs(eigens[i]) > fabs(eigens[j]))
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

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

	int   invx = 1,
	      invy = 1, 
	      invz = 1;	

	double nx,
	       ny, 
	       nz;	

	nx =  Ox[0]*Oy[1]*Oz[2]+Ox[1]*Oy[2]*Oz[0]+Oy[0]*Oz[1]*Ox[2]-Ox[2]*Oz[0]*Oy[1]-Oz[1]*Oy[2]*Ox[0]-Ox[1]*Oy[0]*Oz[2];
	
	if (nx<0)
	{
		Oz[0] = -Oz[0];	
		Oz[1] = -Oz[1];	
		Oz[2] = -Oz[2];	
	}

	for (int i=0; i<entries_num; ++i)
	{
		nx = points[i].x;
		ny = points[i].y;
		nz = points[i].z;
	
          points[i].x = Ox[0]*nx + Ox[1]*ny + Ox[2]*nz;
          points[i].y = Oy[0]*nx + Oy[1]*ny + Oy[2]*nz;
          points[i].z = Oz[0]*nx + Oz[1]*ny + Oz[2]*nz;
	}

	double mx = CalculateMoment(3, 0, 0);
	double my = CalculateMoment(0, 3, 0);

	if ((mx<0) && (my>0))
	{
		invx = -invx;
		invz = -invz;		
	}
			
	if ((mx>0) && (my<0))
	{
		invy = -invy;
		invz = -invz;
	}

	if ((mx<0) && (my<0))
	{
		invy = -invy;
		invx = -invx;
	}
	
	Ox[0] = invx*Ox[0];
	Ox[1] = invx*Ox[1];
	Ox[2] = invx*Ox[2];

	Oy[0] = invy*Oy[0];
	Oy[1] = invy*Oy[1];
	Oy[2] = invy*Oy[2];

	Oz[0] = invz*Oz[0];
	Oz[1] = invz*Oz[1];
	Oz[2] = invz*Oz[2];

	for (int i=0; i<entries_num; ++i)
	{
          points[i].x *= invx;
          points[i].y *= invy;
          points[i].z *= invz;
	}


     tgt::Matrix4d out_data(Ox[0], Ox[1], Ox[2], 0,
					   Oy[0], Oy[1], Oy[2], 0,
 					   Oz[0], Oz[1], Oz[2], 0,
			     	   0,     0,     0,     1);
	return out_data;
}


int PointCloud :: GetMomentsNumber(int order)
{
	if (order == 0) return 1;
	if (order == 1) return 7;

	int  k = 7;

	for (int i=2; i<order+1; ++i)
	{
		k += 4*(i-1)*(i-2) + 12*i-6;		
	}
	return k;
}

void PointCloud :: GetMoments(int order)
{
	mom_order = order;

	GetShift();
	GetAxes();

	int a, b, c;
	int l = 0, k = GetMomentsNumber(order);
	int deg;
	double *temp;

	moments = new double[k];
	
	deg  = 2*mom_order+1;
	for (int i = 0; i < deg*deg*deg; ++i) 
	{
		a = i / (deg * deg);
		b = (i - a*deg*deg) / deg;
		c = i % deg - mom_order;
		a -= mom_order;
		b -= mom_order;
				
		if (sqrt(a*a) + sqrt(b*b) + sqrt(c*c) < (mom_order+1))
		{
			moments[l] = CalculateFourrier(a, b, c);
			l++;
		}
	}	
	if (k!=l)
	{
		printf("Fatal error: Number of moments varies from the expected!\n");
		exit(0);
	}

	mom_total = l;	
	have_moments = true;
}

#include "databasematch.h"

#define 	SCALE		    	30
#define	SOLVE_ITER		52
#define	PI_2	  			1.57079632679
#define	MAX_SIZE			1.5
#define	DB_FILENAME		"database.dat"
#define	NAMELEN			  8
#define	MAX_ORDER			8
#define	NUM_TO_OUT		100

DatabaseMatch :: DatabaseMatch()
  : Processor(),
    sourceselection_("sourcetype", "Select Source", Processor::INVALID_PROGRAM),
    volinport_(Port::INPORT,   "volume",   "Electon density map"),
    molinport_(Port::INPORT,   "molecule", "Molecule structure")

{
    sourceselection_.addOption("mol", "Molecule[coords set]");
    sourceselection_.addOption("vol", "Volume  [Electrun density]");
    addProperty(sourceselection_);
    addPort(molinport_);
    addPort(volinport_);
}

void DatabaseMatch :: process()
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
//		FillOutport();
		delete[] coords;
	}
	if (sourceselection_.isSelected("vol"))
	{
		tgt::svec3 dims   = volinport_.getData()->getDimensions();
		size_t     voxels = volinport_.getData()->getNumVoxels();
		tgt::vec3  space  = volinport_.getData()->getSpacing();
		coords = new double[4*voxels];

		for (int i=0; i<dims.x; ++i)
			for (int j=0; j<dims.y; ++j)
				for (int k=0; k<dims.z; ++k)
				{
					coords[4*(i+j*dims.x+k*dims.x*dims.y)]   = i*space.x;
					coords[4*(i+j*dims.x+k*dims.x*dims.y)+1] = j*space.y;
					coords[4*(i+j*dims.x+k*dims.x*dims.y)+2] = k*space.z;
					coords[4*(i+j*dims.x+k*dims.x*dims.y)+3] = ((VolumeRAM*)volinport_.getData())->getVoxelNormalized(i, j, k);
				}
		PDBFindAxes();
//		PDBFillOutport();
		delete[] coords;
	}		

//   READ THE DATABASE FILE

	int N;
	FILE * db;

	db = fopen(DB_FILENAME, "r");		// open database file
	fscanf(db, "%d", &N);			// read total number of entries
	
	char			buff[8];
	char			*name;
	long double	*disp;
	double		*db_moments;
	double 		dt;
	char   		ct[NAMELEN];
	double         temp;
	unsigned int   n = N, 
				p = n/2, 
				parent, 
				child;  


	name =    new char[N*NAMELEN+1];
	disp =    new long double[N+1];
	db_moments = new double[mom_total+1];
	

	for (int i = 0; i < N; ++i)
	{	
		fscanf(db, "%s", ct);
		strncpy(&name[NAMELEN*i], &ct[0], NAMELEN);
		fscanf(db, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &dt, &dt, &dt, &dt, &dt, &dt, &dt, &dt, &dt, &dt, &dt, &dt); // ingore axes
		disp[i] = 0;
		for (int j=0; j < mom_total; ++j)
		{
			fscanf(db, "%lf ", &db_moments[j]);
			disp[i] += (moments[j] - db_moments[j])*(moments[j] - db_moments[j]);
		}
		disp[i] /= mom_total;
		disp[i] = sqrt(disp[i]);
	}	

	fclose(db); 
  
     while (1) 
     {
		if (p > 0) 
	     { 
     	     p--;          
			temp = disp[p];  
			memcpy(buff, &name[p*NAMELEN], NAMELEN*sizeof(char));
		} 
		else 
		{
           	n--;        
            	if (n == 0) break; 
			temp = disp[n];  
			memcpy(buff, &name[n*NAMELEN], NAMELEN*sizeof(char));   
			disp[n] = disp[0];
			memcpy(&name[n*NAMELEN], &name[0], NAMELEN*sizeof(char));
          }  
        	parent = p; 
        	child = p*2 + 1; 
        	while (child < n) 
	   	{	  
          	if (child + 1 < n  &&  disp[child+1] > disp[child]) 
			{  
	               child++;
	          }  

        		if (disp[child] > temp) 
			{
				disp[parent] = disp[child];
				memcpy(&name[parent], &name[child], NAMELEN*sizeof(char));
               	parent = child;   
	               child = parent*2 + 1;
     	     } 
			else 
			{  
               	break; 
          	}  
        }  
	   disp[parent] = temp;
   	   memcpy(&name[parent], buff, NAMELEN*sizeof(char));
     }

	for (int i=0; i<NUM_TO_OUT; ++i)
	{
		printf("%10s %15Lf\n", &name[i], disp[i]);
	}	

	delete[] name;
	delete[] disp;
	delete[] db_moments;
	delete[] coords; 
}

double DatabaseMatch :: CalculateMoment(int degX, int degY, int degZ)
{
	double res = 0;
	double temp;
	for (int i=0; i<entries; ++i)
	{
		temp = coords[4*i+3];
		for (int j = 0; j < degX; ++j)
		{
			temp *= coords[4*i];
		}
		
		for (int j = 0; j < degY; ++j)
		{
			temp *= coords[4*i+1];
		}
		
		for (int j = 0; j < degZ; ++j)
		{
			temp *= coords[4*i+2];
		}
		res += temp/total_weight;
	}
	return res;
}

double DatabaseMatch :: CalculateFourrier(int degX, int degY, int degZ)
{
	double res = 0;
	double temp;
	for (int i=0; i<entries; ++i)
	{
		temp = coords[4*i+3];
		if (degX<0) temp *= cos(degX*coords[4*i]  /MAX_SIZE*PI_2);
		if (degX>0) temp *= sin(degX*coords[4*i]  /MAX_SIZE*PI_2);
		if (degY<0) temp *= cos(degY*coords[4*i+1]/MAX_SIZE*PI_2);
		if (degY>0) temp *= sin(degY*coords[4*i+1]/MAX_SIZE*PI_2);
		if (degZ<0) temp *= cos(degZ*coords[4*i+2]/MAX_SIZE*PI_2);
		if (degZ>0) temp *= sin(degZ*coords[4*i+2]/MAX_SIZE*PI_2);
		res += temp;
	}
	return res;
}

/*void DatabaseMatch :: FillOutport()
{
		WeightedPointCloud* out_data = new WeightedPointCloud(); // Must be a pointer
		
		out_data->add(WeightedPoint(O[0],   O[1],   O[2],  0));
		out_data->add(WeightedPoint(Ox[0],  Ox[1],  Ox[2], 0));
		out_data->add(WeightedPoint(Oy[0],  Oy[1],  Oy[2], 0));
		out_data->add(WeightedPoint(Oz[0],  Oz[1],  Oz[2], 0));
		
		outport_.setData(out_data);
}
*/

void DatabaseMatch :: PDBFindAxes()
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

	for (int i=0; i<entries; ++i)
	{
		V1[0] = coords[4*i];
		V1[1] = coords[4*i+1];
		V1[2] = coords[4*i+2];
		
		coords[4*i]   = Ox[0]*V1[0] + Ox[1]*V1[1] + Ox[2]*V1[2];
		coords[4*i+1] = Oy[0]*V1[0] + Oy[1]*V1[1] + Oy[2]*V1[2];		
		coords[4*i+2] = Oz[0]*V1[0] + Oz[1]*V1[1] + Oz[2]*V1[2];
	}
}

double DatabaseMatch :: PolynomVal(double x)
{
	return polynom[3]*x*x*x + polynom[2]*x*x + polynom[1]*x + polynom[0];
}

void DatabaseMatch :: GetMoments()
{
	int a, b, c;
	int l = 0;
	
	for (int i = 0; i < 2*MAX_ORDER*MAX_ORDER*MAX_ORDER; ++i) 
	{
		a = i / (MAX_ORDER * MAX_ORDER);
		b = (i - a*MAX_ORDER*MAX_ORDER) / MAX_ORDER;
		c = i % MAX_ORDER  - MAX_ORDER + 1;
		a -= MAX_ORDER - 1;
		b -= MAX_ORDER - 1;
		if ((sqrt(a*a)+sqrt(b*b)+sqrt(c*c) < MAX_ORDER))
		{
			moments[l] = CalculateFourrier(a, b, c);
			l++;
		}
	}	
	mom_total = l;
}

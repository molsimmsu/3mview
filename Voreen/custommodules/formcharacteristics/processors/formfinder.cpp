#include "formfinder.h"


std::string FormFinder::loggerCat_ = "homology.FormFinder";

FormFinder::FormFinder()
    : findDomains_("findDomains", "Find Domains")
    , volinport_(Port::INPORT,   "volume", "Electon density map")
    , alignmentList_("alignmentList", "Alignment List")
    , maxDomainsToLoad_("maxDomainsToLoad", "Max domains to load", 3, 1, 5)
{
    addProperty(findDomains_);
    addProperty(maxDomainsToLoad_);
   
    addPort(volinport_);
    findDomains_.onChange(CallMemberAction<FormFinder>(this, &FormFinder::findDomains));
}

void FormFinder::findDomains() 
{
  O[0] = 0;
	O[1] = 0;
	O[2] = 0;
     total_weight = 0;

	float valNorm = 0;
	float valRW   = 0;
	const VolumeBase* vol    =  volinport_.getData();
	if (vol==0) return;
	const VolumeRAM* volRam = vol->getRepresentation<VolumeRAM>();

	RealWorldMapping    rwm = vol->getRealWorldMapping();

	tgt::svec3  dims   =  vol->getDimensions();
	tgt::vec3   space  =  vol->getSpacing();
	tgt::vec4   pWorld;


	entries = vol->getNumVoxels();
	coords  = new float[4*entries];


	for (int i=0; i<dims.x; ++i)
		for (int j=0; j<dims.y; ++j)
			for (int k=0; k<dims.z; ++k)
			{
				pWorld = vol->getVoxelToWorldMatrix() * (tgt::vec4(0.5+i, 0.5+j, 0.5+k, 1.0));
				coords[4*(i+j*dims.x+k*dims.x*dims.y)]   = pWorld.x;
				coords[4*(i+j*dims.x+k*dims.x*dims.y)+1] = pWorld.y;
				coords[4*(i+j*dims.x+k*dims.x*dims.y)+2] = pWorld.z;
				valNorm = volRam->getVoxelNormalizedLinear(tgt::vec3(0.5+i, 0.5+j, 0.5+k));
				valRW = rwm.normalizedToRealWorld(valNorm);
				coords[4*(i+j*dims.x+k*dims.x*dims.y)+3] = valRW;				
                   }

	for (int i=0; i<entries; ++i)
	{	
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
	GetMoments();
	delete[] coords;	

//   READ THE DATABASE FILE

	int N;
	FILE * db;

	db = fopen(DB_FILENAME, "r");		// open database file
	if (db==NULL)
	{
		LINFO("failed to open database file!");
		return;
	}
	LINFO("Moments database file opened");
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
	//	printf("%d --- %s\n", i , ct);
		strncpy(&name[NAMELEN*i], ct, NAMELEN);
		fscanf(db, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &dt, &dt, &dt, &dt, &dt, &dt, &dt, &dt, &dt, &dt, &dt, &dt); // ingore axes
		disp[i] = 0;
		for (int j=0; j < mom_total; ++j)
		{
			fscanf(db, "%lf ", &db_moments[j]);
			disp[i] += (moments[j]/12 - db_moments[j])*(moments[j]/12 - db_moments[j]);
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
				memcpy(&name[NAMELEN*parent], &name[NAMELEN*child], NAMELEN*sizeof(char));
               	parent = child;   
	               child = parent*2 + 1;
     	     } 
			else 
			{  
               	break; 
          	}  
        }  
	   disp[parent] = temp;
   	   memcpy(&name[NAMELEN*parent], buff, NAMELEN*sizeof(char));
     }	


    MoleculeCollectionSource* molCollection = getSourceProcessor();
    if (molCollection == 0) return;
    
    std::string path("../../DomainDB/domains/");
    std::string pdbPath;

    LINFO("Loading domains:");
    for (size_t i = 0; i < maxDomainsToLoad_.get(); i++) {
        pdbPath = path;
	   for (int k=0; k<7; ++k)
	   {
               pdbPath += name[i*NAMELEN+k];
	   }
	   pdbPath += ".pdb";
        LINFO(pdbPath.c_str());
        molCollection->load(pdbPath);
    }
    delete[] name;
    delete[] disp;
    delete[] db_moments;
}


void FormFinder :: FindAxes()
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
		LINFO("Eigenvalues are complex");
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

double FormFinder :: CalculateMoment(int degX, int degY, int degZ)
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

double FormFinder :: CalculateFourrier(int degX, int degY, int degZ)
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

double FormFinder :: PolynomVal(double x)
{
	return polynom[3]*x*x*x + polynom[2]*x*x + polynom[1]*x + polynom[0];
}

void FormFinder :: GetMoments()
{
	int a, b, c;
	int l = 0;

	for (int i = -MAX_ORDER*MAX_ORDER*MAX_ORDER; i < MAX_ORDER*MAX_ORDER*MAX_ORDER; ++i) 
	{
		a = i / (MAX_ORDER * MAX_ORDER);
		b = (i - a*MAX_ORDER*MAX_ORDER) / MAX_ORDER;
		c = i % MAX_ORDER;
		if (sqrt(a*a) + sqrt(b*b) + sqrt(c*c) < MAX_ORDER)
		{
			moments[l] = CalculateFourrier(a, b, c);
			l++;
		}
	}	
   // 239  of 8
	printf("total moments: %d\n", l);
	mom_total = l;
}

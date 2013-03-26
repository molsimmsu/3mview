#include "pdb.h"

#define PACKET_SIZE 1000

void NormVect(double Arg[3])
{
     double len = sqrt(Arg[0]*Arg[0]+Arg[1]*Arg[1]+Arg[2]*Arg[2]);
	Arg[0] /= len;
	Arg[1] /= len;
	Arg[2] /= len;
}

PDB :: PDB()
{
	entries = 0;
}

PDB :: PDB(char * arg)
{
	entries = 0;
	memset(O,  0, 3*sizeof(double));
	memset(Ox, 0, 3*sizeof(double));
	memset(Oy, 0, 3*sizeof(double));
	memset(Oz, 0, 3*sizeof(double));

	int counter;

	MPI_Init (NULL, NULL);	/* starts MPI */
     MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
     MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */

	if (rank == 0)
	{
		Read(arg);
		Center();
		Reorientate();
		MPI_Bcast(&entries, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&total_weight, 1, MPI_INT, 0, MPI_COMM_WORLD);

		MPI_Bcast(atoms, entries*sizeof(particle), MPI_CHAR, 0, MPI_COMM_WORLD);
	}
	else
	{
		MPI_Bcast(&entries, 1, MPI_INT, 0, MPI_COMM_WORLD);;
		MPI_Bcast(&total_weight, 1, MPI_INT, 0, MPI_COMM_WORLD);
		atoms = new particle[entries];
		MPI_Bcast(atoms, entries*sizeof(particle), MPI_CHAR, 0, MPI_COMM_WORLD);
	}
	GetMoments();
}

void PDB :: Read(char * arg1)
{
	strcpy(name, arg1);
	bool flag = false;
	int  k = 0;
	char buff[100];
	char record[7];
	char arg[100];

	record[6] = '\0';

	sprintf(arg, "%s.pdb", name);
	FILE * inp; 
	inp = fopen(arg, "r");
// 1. make sure the file is opened
	if (inp == NULL)
	{
		printf("File %s not found. Exit.\n", arg);
		exit(1);
	}
// 2. reading entries entries
	while (!(feof(inp)))
	{
		memset(record, ' ', sizeof(char)*6);
		fscanf(inp, "%c%c%c%c%c%c", &record[0], &record[1], &record[2], &record[3], &record[4], &record[5]);
		if (!(strcmp(record, "ATOM  ")&&strcmp(record, "HETATM")))
		{
			entries++;
		}
		k=0;
		while ((record[0]!='\n')&&(k<100))
		{
			fscanf(inp, "%c", record);	
			k++;	
		}
	}
	if (entries == 0)
	{
		printf("No ATOM or HETATM entries found. Exit.\n", arg);
		exit(2);		
	}
// 3. OK, reopening file and reading everything
	fclose(inp);
	inp = fopen(arg, "r");
	atoms = new particle[entries];
	entries = 0;
	int het = 0;
	int nz  = 0;
	total_weight = 0;
	while (!(feof(inp)))
	{	
		memset(record, ' ', sizeof(char)*6);
		fscanf(inp, "%c%c%c%c%c%c", &record[0], &record[1], &record[2], &record[3], &record[4], &record[5]);
		if (!(strcmp(record, "ATOM  ")&&(strcmp(record, "HETATM"))))
		{
			het = 0;
			nz  = 0;
			if (strcmp(record, "HETATM")==0) {het = 1;}
			fscanf(inp, "%c%c%c%c%c%c", &atoms[entries].number[0], &atoms[entries].number[1], &atoms[entries].number[2], &atoms[entries].number[3], &atoms[entries].number[4], &atoms[entries].number[5]);	
			fscanf(inp, "%c%c%c%c", &atoms[entries].type[0], &atoms[entries].type[1], &atoms[entries].type[2], &atoms[entries].type[3]);  // Reading  atom type
			fscanf(inp, "%c", atoms[entries].altloc);
			fscanf(inp, "%c%c%c%c", &atoms[entries].residue[0], &atoms[entries].residue[1], &atoms[entries].residue[2], &atoms[entries].residue[3]);
			fscanf(inp, "%c", atoms[entries].chain);
			fscanf(inp, "%c%c%c%c", &atoms[entries].resseq[0], &atoms[entries].resseq[1], &atoms[entries].resseq[2], &atoms[entries].resseq[3]);
			fscanf(inp, "%c", atoms[entries].icode);

		     atoms[entries].number[6] = '\0';
		     atoms[entries].type[4]   = '\0';	
			atoms[entries].altloc[1] = '\0';
			atoms[entries].residue[4]= '\0';
			atoms[entries].chain[1]  = '\0';
			atoms[entries].resseq[4] = '\0';
			atoms[entries].icode[1]  = '\0';

			while (atoms[entries].type[nz]==' ')
			{
				nz++;
			}
///////////////////////////////////////////////////////
//// ASSIGN WEIGHT ////////////////////////////////////
///////////////////////////////////////////////////////

			switch (atoms[entries].type[nz])	
			{
				case 'C':
					atoms[entries].weight = C_WEIGHT;
				break;
				case 'N':
					atoms[entries].weight = N_WEIGHT;
				break;
				case 'O':
					atoms[entries].weight = O_WEIGHT;
				break;
				case 'H':
					atoms[entries].weight = H_WEIGHT;
				break;
				case 'P':
					atoms[entries].weight = P_WEIGHT;
				break;
				case 'S':
					atoms[entries].weight = S_WEIGHT;
				break;
				case 'F':
					atoms[entries].weight = F_WEIGHT;
				break;
				case 'Z':
					atoms[entries].weight = Z_WEIGHT;
				break;
				case 'M':
					if (atoms[entries].type[nz+1] == 'N')
					{
						atoms[entries].weight = MN_WEIGHT;
						break;
					}
					else	if (atoms[entries].type[nz+1] == 'G')
					{
						atoms[entries].weight = MG_WEIGHT;
						break;
					}					
				default:
		//			printf("Warning: [%d] uncommon atom (%s) shall be ignored.\n", entries+1, atoms[entries].type);
					atoms[entries].weight = UNCOMMON_WEIGHT;
			}
			if (het == 1)
			{
					atoms[entries].weight = UNCOMMON_WEIGHT;
			}
/////////////////////////////////////////
/////////////////////////////////////////
			fscanf(inp, "%lf %lf %lf", &(atoms[entries].x), &(atoms[entries].y), &(atoms[entries].z));
			total_weight += atoms[entries].weight; 
			entries++;
		}
		k = 0;
		while ((record[0]!='\n')&&(k<100))
		{
			fscanf(inp, "%c", record);	
			k++;	
		}
	}
	fclose(inp);
}

void PDB :: WritePDB()
{
	if (rank == 0)
	{
		FILE * out;
		char buff[50];
		sprintf(buff, "%s_out.pdb", name);
		out = fopen(buff, "w+");
		if (out == NULL) 
		{
			printf("Failed to open/create %s. Exit.\n", buff);
			exit(3);
		}
		for (int i=0; i<entries; ++i)
		{
				fprintf(out, "ATOM  %6s%4s%1s%4s%1s%4s%1s    %7.3lf %7.3lf %7.3lf\n", atoms[i].number, atoms[i].type, atoms[i].altloc, atoms[i].residue, atoms[i].chain, atoms[i].resseq, atoms[i].icode, atoms[i].x*SCALE, atoms[i].y*SCALE, atoms[i].z*SCALE);	

		}	
		fclose(out);
	}
}

double PDB :: CalculateMoment(int degX, int degY, int degZ)
{
	double res = 0;
	double temp;
	for (int i=0; i<entries; ++i)
	{
		temp = atoms[i].weight;
		for (int j = 0; j < degX; ++j)
		{
			temp *= atoms[i].x;
		}
		
		for (int j = 0; j < degY; ++j)
		{
			temp *= atoms[i].y;
		}
		
		for (int j = 0; j < degZ; ++j)
		{
			temp *= atoms[i].z;
		}

		res += temp/total_weight;
	}
	return res;
}

double PDB :: CalculateFourrier(int degX, int degY, int degZ)
{
	double res = 0;
	double temp;
	for (int i=0; i<entries; ++i)
	{
		temp = atoms[i].weight;
		if (degX<0) temp *= cos(degX*atoms[i].x/MAX_SIZE*PI_2);
		if (degX>0) temp *= sin(degX*atoms[i].x/MAX_SIZE*PI_2);
		if (degY<0) temp *= cos(degY*atoms[i].y/MAX_SIZE*PI_2);
		if (degY>0) temp *= sin(degY*atoms[i].y/MAX_SIZE*PI_2);
		if (degZ<0) temp *= cos(degZ*atoms[i].z/MAX_SIZE*PI_2);
		if (degZ>0) temp *= sin(degZ*atoms[i].z/MAX_SIZE*PI_2);
		res += temp;
	}
	return res;
}

double PDB :: CalculateSpherical(int degX, int degY, int degZ)
{
	double res = 0;
	double temp;
	for (int i=0; i<entries; ++i)
	{
		temp = atoms[i].weight;

	}
	return res;
}

void PDB :: WriteMoments()
{
	if (rank == 0)	
	{
		printf("\n%4s ", name);
		printf("%13lf %13lf %13lf ",  O[0],  O[1],  O[2]);
		printf("%13lf %13lf %13lf ", Ox[0], Ox[1], Ox[2]);
		printf("%13lf %13lf %13lf ", Oy[0], Oy[1], Oy[2]);
		printf("%13lf %13lf %13lf ", Oz[0], Oz[1], Oz[2]);

		for (int k=0; k<mom_total; ++k)
		{
			printf("%13lf ", moments[k]);
		}
	}
}

void PDB :: Center()
{
	//  FIND THE MASS CENTER
	O[0] = 0;
	O[1] = 0;
	O[2] = 0;
	for (int i=0; i<entries; ++i)
	{
		O[0] += atoms[i].x*atoms[i].weight;
		O[1] += atoms[i].y*atoms[i].weight;
		O[2] += atoms[i].z*atoms[i].weight;
	}
	O[0] /= total_weight;
	O[1] /= total_weight;
	O[2] /= total_weight;
	
	//  Let's shift everyone to center the system
	for (int i=0; i<entries; ++i)
	{
		atoms[i].x -= O[0];
		atoms[i].y -= O[1];
		atoms[i].z -= O[2];
	}
}


void PDB :: Reorientate()
{

	for (int i=0; i<entries; ++i)
	{
		atoms[i].x /= SCALE;
		atoms[i].y /= SCALE;
		atoms[i].z /= SCALE;
	}

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

	int   invx = SCALE,
	      invy = SCALE, 
	      invz = SCALE;	

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

	for (int i=0; i<entries; ++i)
	{
		nx = atoms[i].x;
		ny = atoms[i].y;
		nz = atoms[i].z;
	
          atoms[i].x = Ox[0]*nx + Ox[1]*ny + Ox[2]*nz;
          atoms[i].y = Oy[0]*nx + Oy[1]*ny + Oy[2]*nz;
          atoms[i].z = Oz[0]*nx + Oz[1]*ny + Oz[2]*nz;
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

	for (int i=0; i<entries; ++i)
	{
          atoms[i].x *= invx;
          atoms[i].y *= invy;
          atoms[i].z *= invz;
	}
}

double PDB :: PolynomVal(double x)
{
	return polynom[3]*x*x*x + polynom[2]*x*x + polynom[1]*x + polynom[0];
}

void   PDB :: GetMoments()
{
	int a, b, c;
	int l = 0, k = 0, s = 0;
	int deg, piece;
	double *temp;
	
	k = 7;
	for (int i=2; i<MAX_ORDER+1; ++i)
	{
		k += 4*(i-1)*(i-2) + 12*i-6;		
	}

	moments = new double[k];
	memset(moments, 0, k*sizeof(double));

	deg  = 2*MAX_ORDER+1;
	for (int i = 0; i < deg*deg*deg; ++i) 
	{
		a = i / (deg * deg);
		b = (i - a*deg*deg) / deg;
		c = i % deg - MAX_ORDER;
		a -= MAX_ORDER;
		b -= MAX_ORDER;
			

	
		if (sqrt(a*a) + sqrt(b*b) + sqrt(c*c) < (MAX_ORDER+1))
		{
			if (l % size == rank)
			{
				moments[l] = CalculateFourrier(a, b, c);
				s++;		
			}
			++l;
		}
	}	

	if (k!=l)
	{
		printf("Fatal error: Number of moments varies from the expected!\n");
		exit(0);
	}


	if (rank == 0)
	{
		temp    = new double[k];
		memcpy(temp, moments, k*sizeof(double));
		MPI_Reduce(temp, moments, l, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);	
		mom_total = l;
		delete[] temp;
	}
	else
	{
		MPI_Reduce(moments, temp, l, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);	
		delete[] moments;
	}
}	

PDB :: ~PDB()
{
     MPI_Finalize();
	delete[] atoms;
}

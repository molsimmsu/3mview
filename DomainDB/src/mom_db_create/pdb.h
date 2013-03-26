//	Moments calculation application, September, 17 2012	
//                     

#include <stdio.h>
#include <cmath>
#include <cstdlib>
#include <string.h>

/*  MAX_ORDER defines maximum degree of moments to be calculated                           */
/*  WEIGHT_RENORM defines wether weight renormalization of moments is to be performed      */  

#define    MAX_ORDER  		13
#define    SCALE              30
#define	 PI_2		 	1.570796327
#define    MAX_SIZE			1.5     // for fourrier 

/*  atoms weights register  */
#define    C_WEIGHT           12
#define    N_WEIGHT 		14
#define    O_WEIGHT 		16
#define    H_WEIGHT 		1
#define    F_WEIGHT 		0
#define    P_WEIGHT 		0
#define    S_WEIGHT 		32
#define    MG_WEIGHT 		0
#define    MN_WEIGHT 		0
#define    Z_WEIGHT			0
#define    UNCOMMON_WEIGHT 	0



struct particle {
	int    weight;



	double x, y, z;


	char   number[7];
	char   type[5];
	char   altloc[2];
	char   residue[5];
	char   chain[2];
	char   resseq[5];
	char   icode[2];
};

class PDB {
public:
	particle *atoms;
	int 		mom_total;

	int 	  entries;
	char   name[10];
	int 	  total_weight;
	double    scale;
		
	double    Ox[3];
	double    Oy[3];
	double    Oz[3];
	double     O[3];
	
	
	double    I[3][3];
	double    polynom[4];
	double    eigens[3];
	
	double    *moments;

	
	PDB();
	PDB(char * arg);
	~PDB();

	void Read(char * arg);
	void WritePDB();
	
	double CalculateMoment(int degX, int degY, int degZ);
	double CalculateFourrier(int degX, int degY, int degZ);
	double CalculateSpherical(int degX, int degY, int degZ);
	double PolynomVal(double x);

	void   Center();
	void   Rescale();
	void   Reorientate();
	void   Reduce();
	void   GetMoments();
	void   WriteMoments();
};

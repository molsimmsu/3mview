#include "volvolalign.h"


#define    SCALE          300
#define    SOLVE_ITER     60
#define    PI_2           1.57079632679
#define    MAX_SIZE	      1.5    // range of the first harmonic


const std::string VolVolAlign::loggerCat_("3mview.VolVolAlign");

tgt::vec3 getVolumeMassCenter(VolumeBase* vol)
{
	tgt::vec3 mass_center;
	mass_center.x =0.0;
	mass_center.y =0.0;
	mass_center.z =0.0;
	double weight = 0;

	const VolumeRAM* volRam = vol->getRepresentation<VolumeRAM>();

	tgt::svec3 dims   = vol->getDimensions();
	tgt::vec3  space  = vol->getSpacing();

	tgt::vec4     pWorld;

	RealWorldMapping rwm = vol->getRealWorldMapping();
	float valNorm;
	float valRW;

	for (int i=0; i<dims.x; ++i)
		for (int j=0; j<dims.y; ++j)
			for (int k=0; k<dims.z; ++k)
			{
				pWorld = vol->getVoxelToWorldMatrix() * (tgt::vec4(0.5+i, 0.5+j, 0.5+k, 1.0));
				valNorm = volRam->getVoxelNormalizedLinear(tgt::vec3(0.5+i, 0.5+j, 0.5+k));
				valRW = rwm.normalizedToRealWorld(valNorm);

				mass_center.x += pWorld.x * valRW;
				mass_center.y += pWorld.y * valRW;
				mass_center.z += pWorld.z * valRW;
				weight  += valRW;
			}
     mass_center.x /= weight;
     mass_center.y /= weight;
     mass_center.z /= weight;
     return mass_center;
}


VolVolAlign :: VolVolAlign()
  : Processor(),
    tobealigned_("tobealigned", "Volume to reorientate", Processor::INVALID_PROGRAM),
    align_("align", "Align", Processor::INVALID_PROGRAM),
    volinport1_(Port::INPORT,   "volume1", "Electon density map 1"),
    volinport2_(Port::INPORT,   "volume2", "Electon density map 2"),
    outport_(Port::OUTPORT,   "volume3", "Electon density map")
{
    tobealigned_.addOption("Vol1ToVol2", "Volume 1 to Volume 2");
    tobealigned_.addOption("Vol2ToVol1", "Volume 2 to Volume 1");
    tobealigned_.addOption("Vol1ToOrigin", "Volume 1 to Origin");
    tobealigned_.addOption("Vol2ToOrigin", "Volume 2 to Origin");
    addProperty(tobealigned_);
    addProperty(align_);

    addPort(volinport1_);
    addPort(volinport2_);
    addPort(outport_);
    
    align_.onClick(CallMemberAction<VolVolAlign>(this, &VolVolAlign::align));
}

void VolVolAlign :: align()
{
	if (tobealigned_.isSelected("Vol1ToVol2") || tobealigned_.isSelected("Vol2ToVol1"))
	{
		const VolumeBase* firstVolume;
		const VolumeBase* secondVolume;
		
		if (tobealigned_.isSelected("Vol1ToVol2")) {
		    firstVolume  = volinport1_.getData();
		    secondVolume = volinport2_.getData();
		}
		if (tobealigned_.isSelected("Vol2ToVol1")) {
		    firstVolume  = volinport2_.getData();
		    secondVolume = volinport1_.getData();
		}


		tgt::Matrix4d wrld1 =  firstVolume->getPhysicalToWorldMatrix();
		tgt::Matrix4d wrld2 = secondVolume->getPhysicalToWorldMatrix();
		
         LINFO("Getting transformation matrix for object #1..");
        Volume* firstVolume1 = firstVolume->clone();
		tgt::Matrix4d norm1 = GetTransformation(firstVolume1);	
		firstVolume1->setPhysicalToWorldMatrix(norm1*wrld1);
		tgt::Matrix4d norm1a = GetTransformation(firstVolume1);
		norm1=norm1a*norm1;
		
         LINFO("Getting transformation matrix for object #2..");
         Volume* secondVolume2 = secondVolume->clone();
		tgt::Matrix4d norm2 = GetTransformation(secondVolume2);
		secondVolume2->setPhysicalToWorldMatrix(norm2*wrld2);
		tgt::Matrix4d norm2a = GetTransformation(secondVolume2);
		norm2=norm2a*norm2;

		tgt::Matrix4d nvrt2;
		norm2.invert(nvrt2);
 
	     Volume* combinedVolume = firstVolume->clone();

        
		combinedVolume->setPhysicalToWorldMatrix(nvrt2*norm1*wrld1);


		outport_.setData(combinedVolume);
	}

	/*if (tobealigned_.isSelected("Vol1ToOrigin") || tobealigned_.isSelected("Vol2ToOrigin"))
	{
		const VolumeBase* volume;
		if (tobealigned_.isSelected("Vol1ToOrigin")) 
		    volume = volinport1_.getData();
		if (tobealigned_.isSelected("Vol2ToOrigin")) 
		    volume = volinport2_.getData();


	    
		tgt::Matrix4d wrld2 = volume->getPhysicalToWorldMatrix();



 	    LINFO("Getting transformation matrix for object..");
 	     
		Transform tr = GetTransformation2(volume);
		wrld2 = tr.offset * wrld2;	
		wrld2 = tr.rotate * wrld2;
 
	    Volume* combinedVolume = volume->clone();

		combinedVolume->setPhysicalToWorldMatrix(wrld2);

		outport_.setData(combinedVolume);
	}*/

}

tgt::Matrix4d VolVolAlign :: GetTransformation(const VolumeBase* vol)
{
	O[0] = 0;
	O[1] = 0;
	O[2] = 0;
     total_weight = 0;

	const VolumeRAM* volRam = vol->getRepresentation<VolumeRAM>();

	tgt::svec3 dims   = vol->getDimensions();
	size_t     voxels = vol->getNumVoxels();
	entries =  voxels;

	tgt::dvec4     pWorld;
	coords = new double[4*voxels];

	RealWorldMapping rwm = vol->getRealWorldMapping();
	float valNorm;
	float valRW;

	for (int i=0; i<dims.x; ++i)
		for (int j=0; j<dims.y; ++j)
			for (int k=0; k<dims.z; ++k)
			{
				pWorld = vol->getVoxelToWorldMatrix() * (tgt::dvec4(0.5+i, 0.5+j, 0.5+k, 1.0));
				coords[4*(i+j*dims.x+k*dims.x*dims.y)]   = pWorld.x;
				coords[4*(i+j*dims.x+k*dims.x*dims.y)+1] = pWorld.y;
				coords[4*(i+j*dims.x+k*dims.x*dims.y)+2] = pWorld.z;
				valNorm = volRam->getVoxelNormalizedLinear(tgt::dvec3(0.5+i, 0.5+j, 0.5+k));
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
	delete[] coords;

	tgt::Matrix4d out_data (Ox[0], Ox[1], Ox[2], 0,
				   	    Oy[0], Oy[1], Oy[2], 0,
 					    Oz[0], Oz[1], Oz[2], 0,
			     	    0,     0,     0,     1);	
	
	tgt::Matrix4d out_data1;
	
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

    tgt::Matrix4d out_data2 = out_data*out_data1;
	return out_data2;
}

double VolVolAlign :: CalculateMoment(int degX, int degY, int degZ)
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

void VolVolAlign :: FindAxes()
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

	std::ostringstream message;
	message << "\n"
	        << " " << I[0][0] << " " << I[0][1] << " " << I[0][2] << std::endl 
		   << " " << I[1][0] << " " << I[1][1] << " " << I[1][2] << std::endl 
		   << " " << I[2][0] << " " << I[2][1] << " " << I[2][2] << std::endl; 

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
			if (fabs(eigens[i]) > fabs(eigens[j]))
			{
				disc = eigens[i];
				eigens[i] = eigens[j];
				eigens[j] = disc;
			}
		}
	}	
	
	message << "\n " << eigens[0] << " " << eigens[1] << " " << eigens[2] << std::endl;


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
	
	double m300 = CalculateMoment(3, 0, 0);
	double m030 = CalculateMoment(0, 3, 0);
	
	std::cout << "disc is " << disc << std::endl;
	std::cout << "moment300 is " << m300 << std::endl;
	std::cout << "moment030 is " << m030 << std::endl;
	
	if (disc<0)
	{
		Oz[0] = -Oz[0];
		Oz[1] = -Oz[1];
		Oz[2] = -Oz[2];
	}
	
	if (m300<0)
	{
		Ox[0] = -Ox[0];
		Ox[1] = -Ox[1];
		Ox[2] = -Ox[2];

		Oy[0] = -Oy[0];
		Oy[1] = -Oy[1];
		Oy[2] = -Oy[2];
				
	    if (m030>0)
	    {
		    Oz[0] = -Oz[0];
		    Oz[1] = -Oz[1];
		    Oz[2] = -Oz[2];

		    Oy[0] = -Oy[0];
		    Oy[1] = -Oy[1];
		    Oy[2] = -Oy[2];
	    }
	}
	else
	{
	    if (m030<0)
	    {
		    Oz[0] = -Oz[0];
		    Oz[1] = -Oz[1];
		    Oz[2] = -Oz[2];

		    Oy[0] = -Oy[0];
		    Oy[1] = -Oy[1];
		    Oy[2] = -Oy[2];
	    }
	}

	message << "\n"
	        << " " << Ox[0] << " " << Ox[1] << " " << Ox[2] << std::endl 
		   << " " << Oy[0] << " " << Oy[1] << " " << Oy[2] << std::endl 
		   << " " << Oz[0] << " " << Oz[1] << " " << Oz[2] << std::endl; 

	LINFO(message.str().c_str());
}

double VolVolAlign :: PolynomVal(double x)
{
	return polynom[3]*x*x*x + polynom[2]*x*x + polynom[1]*x + polynom[0];
} 

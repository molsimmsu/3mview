#include "volvolalign.h"


#define    SCALE          30
#define    SOLVE_ITER     52
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

	        LINFO("Getting transformation matrix for object #1..");
		tgt::mat4 norm1 = GetTransformation(firstVolume);	
                LINFO("Getting transformation matrix for object #2..");
		tgt::mat4 norm2 = GetTransformation(secondVolume);
		tgt::mat4 nvrt2;
		norm2.invert(nvrt2);
 
	        Volume* combinedVolume = firstVolume->clone();

		tgt::mat4 wrld1 =  firstVolume->getVoxelToWorldMatrix();
        
		combinedVolume->setPhysicalToWorldMatrix(nvrt2*norm1*wrld1); // Venia
		//combinedVolume->setPhysicalToWorldMatrix(wrld2*nvrt2*norm1); // Alexey
		
		
		combinedVolume->setOffset(tgt::vec3(0,0,0));
		combinedVolume->setSpacing(tgt::vec3(1,1,1));
		outport_.setData(combinedVolume);
	}

	if (tobealigned_.isSelected("Vol1ToOrigin") || tobealigned_.isSelected("Vol2ToOrigin"))
	{
		const VolumeBase* volume;
		if (tobealigned_.isSelected("Vol1ToOrigin")) 
		    volume = volinport1_.getData();
		if (tobealigned_.isSelected("Vol2ToOrigin")) 
		    volume = volinport2_.getData();

	    LINFO("Getting transformation matrix for object..");
		tgt::mat4 norm = GetTransformation(volume);	
 
	    Volume* combinedVolume = volume->clone();
	    
		tgt::Matrix4d wrld = volume->getVoxelToWorldMatrix();
		combinedVolume->setPhysicalToWorldMatrix(norm*wrld);

		combinedVolume->setOffset(tgt::vec3(0,0,0));
		combinedVolume->setSpacing(tgt::vec3(1,1,1));
		outport_.setData(combinedVolume);
	}

}

tgt::mat4 VolVolAlign :: GetTransformation(const VolumeBase* vol)
{
	O[0] = 0;
	O[1] = 0;
	O[2] = 0;
     total_weight = 0;

	const VolumeRAM* volRam = vol->getRepresentation<VolumeRAM>();

	tgt::svec3 dims   = vol->getDimensions();
	size_t    voxels = vol->getNumVoxels();
	tgt::vec3  space  = vol->getSpacing();
	entries = voxels;

	tgt::vec4     pWorld;
	coords = new float[4*voxels];

	RealWorldMapping rwm = vol->getRealWorldMapping();
	float valNorm;
	float valRW;

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

double VolVolAlign :: CalculateFourrier(int degX, int degY, int degZ)
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

double VolVolAlign :: PolynomVal(double x)
{
	return polynom[3]*x*x*x + polynom[2]*x*x + polynom[1]*x + polynom[0];
} 

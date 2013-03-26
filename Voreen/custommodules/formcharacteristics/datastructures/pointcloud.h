#ifndef VRN_POINTCLOUD_H
#define VRN_POINTCLOUD_H

#define PI_2 1.57079632679

#include "voreen/core/datastructures/volume/volume.h"
#include "tgt/matrix.h"
#include "tgt/vector.h"
#include "../../molecule/datastructures/molecule.h"
namespace voreen {

class PointCloud {
public:
	int       entries_num;

	tgt::vec3 *points;
	double    *values;
	double    scale;
	double    max_size;   // maximum of

	double    max_value;  // max value for volumes
	double    min_value;  // min value (cutoff)
 
	double    weightfactor;

	double    *moments;
	int        mom_total;

	tgt::Matrix4d GetShift();
	tgt::Matrix4d GetAxes();

	void GetMoments(int order);
	int  GetMomentsNumber(int order);


	double CalculateMoment(int, int, int);   	
	double CalculateFourrier(int, int, int); 

	void VolumeFill(const Volume* vol, double min);  // min = min_value
	void VolumeFill(const Volume* vol);  // min = 0

 	void MoleculeFill(const Molecule* mol);

	void SetOrientation(tgt::mat4 arg);
	

	PointCloud();
	~PointCloud();
private:


	bool  have_points;
	bool  have_moments;

	float O[3]; 
	float Ox[3];
	float Oy[3];
	float Oz[3];

	double polynom[4];
	double weight;
	int    orientation;
	int    mom_order;

	int    stepx; 	
	int    stepy;
	int    stepz;
	
	double PolynomVal(double);
	
	void Centrify();
	void Reorientate();
};

}
#endif 

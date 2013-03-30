#include "correlation.h"

#include "voreen/core/processors/processorwidget.h"
#include "voreen/core/processors/processorwidgetfactory.h"
#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/datastructures/volume/volumecollection.h"
#include "voreen/core/ports/allports.h"
#include "voreen/core/io/volumeserializerpopulator.h"
#include "voreen/core/io/volumeserializer.h"
#include "voreen/core/io/progressbar.h"
#include "voreen/core/voreenapplication.h"
#include "openbabel/typer.h"
#include "openbabel/data.h"

#include <fstream>
#include <iostream>
#include <math.h>
#include <assert.h>
#include <cmath>

#include "tgt/exception.h"
#include "tgt/vector.h"
#include "tgt/texturemanager.h"

#include "voreen/core/datastructures/volume/volumeatomic.h"

#define PI 3.141592

#define REBUILD_GRID CallMemberAction<Correlation>(this, &Correlation::Calculate)
using std::string;
using tgt::vec3;
using tgt::ivec3;
using tgt::Texture;
using namespace OpenBabel;

namespace voreen {





const std::string Correlation::loggerCat_("voreen.core.Correlation");

Correlation::Correlation()
    : Processor()
, SingleVolumeInport_(Port::INPORT, "SingleVolumeInput", "Single Volume Input")
, SingleVolumeOutport_(Port::OUTPORT, "SingleVolumeOutport", "Single Volume Outport")
, resol_("Resolution", "NumberOfVoxelPerVosels", 1, 0, 100)
, calcblur_("calcblur", "Calculate")


{
addPort(SingleVolumeInport_);
addPort(SingleVolumeOutport_);

addProperty(resol_);
addProperty(calcblur_);

calcblur_.onClick(REBUILD_GRID);

}

Correlation::~Correlation() {
}

Processor* Correlation::create() const {
        return new Correlation();
}

void Correlation::Calculate()
{
const VolumeBase* SingleInput = SingleVolumeInport_.getData();
const VolumeRAM* SingleInputVolume = SingleInput->getRepresentation<VolumeAtomic<float_t> >();

ivec3 NN=SingleInputVolume->getDimensions();
int N=resol_.get();

VolumeRAM* targetDataset = new VolumeAtomic<float_t>(ivec3(NN[0],NN[1],NN[2]));

float gaus[2*N+1][2*N+1][2*N+1],sigm2,meanval;
int tempi,tempj,tempk;
sigm2=pow(N/3.0,2);
if (N!=0)
{

float summ=0;
for (int i=-N; i<=N; i++)
    for (int j=-N; j<=N; j++)
    for (int k=-N; k<=N; k++)
    {
        tempi=i+N;
        tempj=j+N;
        tempk=k+N;
    gaus[tempi][tempj][tempk]=1/pow((2*PI*sigm2),1.5)*exp(-(i*i+j*j+k*k)/(2*sigm2));
    summ=summ+gaus[tempi][tempj][tempk];

    }

for (int i=-N; i<=N; i++)
    for (int j=-N; j<=N; j++)
    for (int k=-N; k<=N; k++)
    {
        tempi=i+N;
        tempj=j+N;
        tempk=k+N;
    gaus[tempi][tempj][tempk]=gaus[tempi][tempj][tempk]/summ;

    }

}

else gaus[0][0][0]=1;

for (int i=0; i<NN[0]; i++)
{

for (int j=0; j<NN[1]; j++)
for (int k=0; k<NN[2]; k++)
{
    meanval=0;
    for (int ii=-N; ii<=N; ii++)
    for (int jj=-N; jj<=N; jj++)
    for (int kk=-N; kk<=N; kk++)
    {
        tempi=i+ii;
        tempj=j+jj;
        tempk=k+kk;
        if ((tempi>=0)&(tempj>=0)&(tempk>=0)&(tempi<NN[0])&(tempj<NN[1])&(tempk<NN[2]))

        meanval=meanval+((VolumeAtomic<float_t>*)SingleInputVolume)->voxel(tempi,tempj,tempk)*gaus[ii+N][jj+N][kk+N];

    }

 ((VolumeAtomic<float_t>*)targetDataset)->voxel(i,j,k)=meanval;
}
std::cout << "Blur...: "<<i*100/NN[0]<< "%"<<"\r";
}




tgt::Matrix4<int> transform
        (
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );

Volume* volumeHandle = new Volume(
            targetDataset,                                                                                // data
            SingleInput->getSpacing(),                                                                      // scale
            SingleInput->getOffset(), // offset
            transform                                                                                     // transform
        );

SingleVolumeOutport_.setData(volumeHandle);
std::cout << "Calculation complited!"<<std::endl;

}

void Correlation::process() {

/*
for (int i=0; i<Nx; i++)
for (int j=0; j<Ny; j++)
for (int k=0; k<Nz; k++)
{


meanval=0;


for (int ii=0; ii<N; ii++)
for (int jj=0; jj<N; jj++)
for (int kk=0; kk<N; kk++)
{
    tempi=i*N+ii;
    tempj=j*N+jj;
    tempk=k*N+kk;
//    if (tempi>=NN[0]) tempi=tempi-NN[0];
//    if (tempj>=NN[0]) tempj=tempj-NN[1];
//    if (tempk>=NN[0]) tempk=tempk-NN[2];

if ((tempi<NN[0])&(tempj<NN[1])&(tempk<NN[2]))
meanval=meanval+((VolumeAtomic<float_t>*)vr2)->voxel(tempi,tempj,tempk);


}
meanval=meanval/pow(N,3);


for (int ii=0; ii<N; ii++)
for (int jj=0; jj<N; jj++)
for (int kk=0; kk<N; kk++)
{
    tempi=i*N+ii;
    tempj=j*N+jj;
    tempk=k*N+kk;

if ((tempi<NN[0])&(tempj<NN[1])&(tempk<NN[2]))
((VolumeAtomic<float_t>*)targetDataset)->voxel(tempi,tempj,tempk)=meanval;
}

}

*/


/*
//calc mean density
vec3 space=inputVolume2->getSpacing();
std::cout<<"calc mean density..."<<std::endl;
for (int i=0; i<NN[0]; i++)
for (int j=0; j<NN[1]; j++)
for (int k=0; k<NN[2]; k++)
{
 //   mean1=mean1+((VolumeAtomic<float_t>*)vr1)->voxel(i,j,k)*pow(space[0],3);
 //  mean2=mean2+((VolumeAtomic<float_t>*)targetDataset)->voxel(i,j,k)*pow(space[0],3);

 mean1=mean1+((VolumeAtomic<float_t>*)vr1)->voxel(i,j,k);
 mean2=mean2+((VolumeAtomic<float_t>*)targetDataset)->voxel(i,j,k);
}
mean1=mean1/(NN[0]*NN[1]*NN[2]);
mean2=mean2/(NN[0]*NN[1]*NN[2]);
std::cout<<"Mean dens in volume1: "<<mean1<<std::endl;
std::cout<<"Mean dens in volume2: "<<mean2<<std::endl;
//----------------



//calc correlation density
std::cout<<"calc correlation..."<<std::endl;
float cov=0,sigmv1=0,sigmv2=0;
for (int i=0; i<NN[0]; i++)
for (int j=0; j<NN[1]; j++)
for (int k=0; k<NN[2]; k++)
{
    cov=cov+(((VolumeAtomic<float_t>*)vr1)->voxel(i,j,k)-mean1)*(((VolumeAtomic<float_t>*)targetDataset)->voxel(i,j,k)-mean2);
    sigmv1=sigmv1+pow( (((VolumeAtomic<float_t>*)vr1)->voxel(i,j,k)-mean1) ,2);
    sigmv2=sigmv2+pow( (((VolumeAtomic<float_t>*)targetDataset)->voxel(i,j,k)-mean2) ,2);
}
float corr=cov/sqrt(sigmv1*sigmv2);

std::cout<<"correlation coefficient: "<<corr<<std::endl;
//----------------

*/



}



} // namespace

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

#define REBUILD_GRID CallMemberAction<Correlation>(this, &Correlation::OnClick)
using std::string;
using tgt::vec3;
using tgt::ivec3;
using tgt::Texture;
using namespace OpenBabel;

namespace voreen {





const std::string Correlation::loggerCat_("voreen.core.Correlation");

Correlation::Correlation()
    : Processor()
, resol_("Resolution", "NumberOfVoxelPerVosels", 1, 0, 20)
, calcblur_("calcblur", "Calculate")
, volumeURLList_("volumeURLList", "Volume URL List", std::vector<std::string>())


{

addProperty(resol_);
addProperty(calcblur_);
addProperty(volumeURLList_);

calcblur_.onClick(REBUILD_GRID);

}

Correlation::~Correlation() {
}

Processor* Correlation::create() const {
        return new Correlation();
}
void Correlation::OnClick()
{
  Volume* volume;
  volume = CalculateBlur();
  if (volume!=0) getSourceProcessor()->addVolume(volume, true, true);
  else LWARNING("Please select volume!");
}

void Correlation::updateSelection() {
    DensityMapCoProcessor::updateSelection();
    const VolumeCollection* collection = getInputVolumeCollection();
    if (collection == 0) {
        LERROR("Collection is NULL at DensityMapManipulation::updateSelection()");
        return;
    }
    volumeURLList_.clear();
    for (size_t i = 0; i < collection->size(); i++)
        volumeURLList_.addVolume(collection->at(i));
}


Volume*  Correlation::CalculateBlur()
{

VolumeCollection* volumes = volumeURLList_.getVolumes(true);

if (volumes->size()!=0)
{

const VolumeBase* SingleInput = volumes->at(0);
const VolumeRAM* SingleInputVolume = SingleInput->getRepresentation<VolumeAtomic<float_t> >();

ivec3 NN=SingleInputVolume->getDimensions();
int N=resol_.get();
VolumeRAM* targetDataset = new VolumeAtomic<float_t>(ivec3(NN[0],NN[1],NN[2]));

LINFO("Create Gauss core...");
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
std::cout << "Blur...: "<<(i+1)*100/NN[0]<< "%"<<"\r";
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

volumeHandle->setOrigin(SingleInput->getOrigin());
LINFO("Calculation complited!");
return volumeHandle;
}
else
{
   return 0;
}
}

void Correlation::process() {}

} // namespace

#include "pdbtoedm.h"

#include "../datastructures/moleculevolume.h"
#include "voreen/core/processors/processorwidget.h"
#include "voreen/core/processors/processorwidgetfactory.h"
#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/datastructures/volume/volumecollection.h"
#include "voreen/core/ports/allports.h"
#include "voreen/core/io/volumeserializerpopulator.h"
#include "voreen/core/io/volumeserializer.h"
#include "voreen/core/io/progressbar.h"
#include "voreen/core/voreenapplication.h"

#include "voreen/core/fft/fftw.h"


#include "openbabel/typer.h"
#include "openbabel/data.h"

#include <fstream>
#include <iostream>
#include <assert.h>
#include <cmath>

#include "tgt/exception.h"
#include "tgt/vector.h"
#include "tgt/texturemanager.h"

#include "voreen/core/datastructures/volume/volumeatomic.h"

#define PI 3.141592
#define REBUILD_EDM_GRID CallMemberAction<PDBtoEDM>(this, &PDBtoEDM::ShowGrid)
//#define SELECT_CALCULATION_MODE CallMemberAction<PDBtoEDM>(this, &PDBtoEDM::adjustPropertyVisibility)

using std::string;
using tgt::vec3;
using tgt::ivec3;
using tgt::Texture;
using namespace OpenBabel;


namespace voreen {




const std::string PDBtoEDM::loggerCat_("voreen.core.PDBtoEDM");

PDBtoEDM::PDBtoEDM()
    : Processor()
    , calculationmode_("calcmode", "Calculation mode")
    , atoomr_("atoomr", "Length of calc (A)", 2, 1, 3)
    , deltaatoomr_("deltaatoomr", "Step (*0.01 A)", 20, 1, 500)
    , resolution_("resolution", "Resolution (*0.1 A)", 30, 1, 100)
    //, gaussvoxel_("gaussvoxel_", "Gauss blur", 1, 0, 5)
    , calcelectronnumb_("calcelectron", "Normal Electron Number", true)
    //, gaussfiltering_("gaussfiltering", "Gauss convolution", false)
    , generategrid_("generategrid", "Generate grid")
    , moleculeURLlist_("moleculeURLlist_", "Molecule URL List", std::vector<std::string>())
{

    calculationmode_.addOption("scattering", "Scattering factor");
    calculationmode_.addOption("structure", "Structure factor");


    //calculationmode_.onChange(SELECT_CALCULATION_MODE);
    addProperty(moleculeURLlist_);
    addProperty(calculationmode_);
    addProperty(atoomr_);
    addProperty(deltaatoomr_);
    addProperty(resolution_);
    addProperty(calcelectronnumb_);
    //addProperty(gaussfiltering_);
    //addProperty(gaussvoxel_);
    addProperty(generategrid_);

    generategrid_.onClick(REBUILD_EDM_GRID);

    LINFO("Loaded successfully");
    LINFO("No conflicts with modules");
}

PDBtoEDM::~PDBtoEDM() {
}

Processor* PDBtoEDM::create() const {
        return new PDBtoEDM();
}

Volume* PDBtoEDM::GenerateEDMGrid_ScatteringFactor(Molecule* InputMoll) {
dr=deltaatoomr_.get()/100.0; //step of grid
MaxR=atoomr_.get();
VoxelPerAngstrem=1.0/dr; //number of voxels per angstrem
int Nsmall=atoomr_.get()/dr; //
float scale=1.0/VoxelPerAngstrem;
struct AtomicED sAtomED;
const OBMol& mol = InputMoll->getOBMol();

std::cout << "Delta_r: "<< dr<<std::endl;
std::cout << "Voxel per angstrem: "<< VoxelPerAngstrem<<std::endl;

//--------find atom types in pdb-----------
FindAtomTypesInPDB(mol, &sAtomED);
//-----------------------------------------


//-----------------------------------------
//--------calc radial ED for atoms---------
//-----------------------------------------

for (int k=0; k<NumberAtom; k++)

{
//calc radial distr. from FT of atomic structure. DWF not include
  for (int j=0; j<Nsmall; j++ )
  {
    sAtomED.AtomED[k][j]=sAtomED.a1[k]*exp(-4*PI*PI*pow(dr*j,2)/sAtomED.b1[k])/pow(sAtomED.b1[k]/(4*PI),1.5)
    +sAtomED.a2[k]*exp(-4*PI*PI*pow(dr*(j),2)/sAtomED.b2[k])/pow(sAtomED.b2[k]/(4*PI),1.5)
    +sAtomED.a3[k]*exp(-4*PI*PI*pow(dr*(j),2)/sAtomED.b3[k])/pow(sAtomED.b3[k]/(4*PI),1.5)
    +sAtomED.a4[k]*exp(-4*PI*PI*pow(dr*(j),2)/sAtomED.b4[k])/pow(sAtomED.b4[k]/(4*PI),1.5);
  }
}
//-----------------------------------------
//-----------------------------------------

//-----------------------------------------
//----------find bounding box--------------
//-----------------------------------------
FindBoundingGeometry(mol);
std::cout << "NumberVoxels_x:"<<NumberVoxels_x<<std::endl;
std::cout << "NumberVoxels_y: "<<NumberVoxels_y<<std::endl;
std::cout << "NumberVoxels_z: "<<NumberVoxels_z<<std::endl;
//-----------------------------------------
//-----------------------------------------
VolumeRAM* targetDataset = new VolumeAtomic<float>(ivec3(NumberVoxels_x,NumberVoxels_y,NumberVoxels_z));


//-----------------------------------------
//----------create out volume--------------
//-----------------------------------------

float R,ED;
for (int i=0; i<NumberVoxels_x; i++)
for (int j=0; j<NumberVoxels_y; j++)
for (int k=0; k<NumberVoxels_z; k++)
{
     ((VolumeAtomic<float>*)targetDataset)->voxel(i,j,k)=0;
}
std::string src,dst;
for (int i = 1; i <= mol.NumAtoms(); i++)
    {
        OBAtom* a = mol.GetAtom(i);
        src=a->GetType();
        ttab.SetFromType("INT");
        ttab.SetToType("XYZ");
        ttab.Translate(dst,src);
        int p=0;
        while (dst!=sAtomED.AtomName[p]) p=p+1;

        float atomx=a->x()+size_x+MaxR-cx;
        float atomy=a->y()+size_y+MaxR-cy;
        float atomz=a->z()+size_z+MaxR-cz;

        int Voxx=atomx/dr;
        int Voxy=atomy/dr;
        int Voxz=atomz/dr;

        for (int ii=-Nsmall+1; ii<Nsmall; ii++)
        for (int jj=-Nsmall+1; jj<Nsmall; jj++)
        for (int kk=-Nsmall+1; kk<Nsmall; kk++)
        {
            int tempVoxx=Voxx+ii;
            int tempVoxy=Voxy+jj;
            int tempVoxz=Voxz+kk;
            int temp1=ii*ii+jj*jj+kk*kk;
            if ((tempVoxx>=0)&(tempVoxy>=0)&(tempVoxz>=0)&(tempVoxx<NumberVoxels_x)&(tempVoxy<NumberVoxels_y)&(tempVoxz<NumberVoxels_z)&(temp1<Nsmall*Nsmall)){

                float temp=((VolumeAtomic<float>*)targetDataset)->voxel(tempVoxx,tempVoxy,tempVoxz);

                if ((ii==0)&(jj==0)&(kk==0))
                {
                R=sqrt(pow(atomx-(tempVoxx*dr+dr/2),2)+pow(atomy-(tempVoxy*dr+dr/2),2)+pow(atomz-(tempVoxz*dr+dr/2),2));
                ED=CalcElectronDens(sAtomED,p,R);
                }
                else
                {
                int pos=sqrt(temp1);
                ED=sAtomED.AtomED[p][pos];
                }


                ((VolumeAtomic<float>*)targetDataset)->voxel(tempVoxx,tempVoxy,tempVoxz)=temp+ED;


            }
        }

std::cout << "Calculate density map: "<<i*100/mol.NumAtoms()<< "%"<<"\r";
    }
//-----------------------------------------
//-----------------------------------------


//-----------------------------------------
//--------Calc number of electrons----------
//-----------------------------------------
if (calcelectronnumb_.get()==true) CalcElectronNumber(targetDataset,NumberOfElectrons);
//-----------------------------------------
//-----------------------------------------


MoleculeVolume* volumeHandle = new MoleculeVolume(
            targetDataset,                                                                                // data
            vec3(scale,scale,scale),                                                                      // scale
            vec3(-(size_x+MaxR)+cx,-(size_y+MaxR)+cy,-(size_z+MaxR)+cz), // offset
            InputMoll->getTransformationMatrix()                                                                                   // transform
        );
        
volumeHandle->setMolecule(InputMoll);

return volumeHandle;

//-----------------------------------------
//-----------------------------------------
}

Volume* PDBtoEDM::GenerateEDMGrid_StructureFactor(Molecule* InputMoll) {

dr=deltaatoomr_.get()/100.0; //step of grid
MaxR=2;
VoxelPerAngstrem=1.0/dr; //number of voxels per angstrem
resol=resolution_.get()/10.0;
struct AtomicED sAtomED;
const OBMol& mol = InputMoll->getOBMol();

//--------find atom types in pdb-----------
FindAtomTypesInPDB(mol, &sAtomED);
//-----------------------------------------

//----------find bounding box--------------
FindBoundingGeometry(mol);
//-----------------------------------------
int NN=2*Nh;
static complex StF[100000000];
float ddd;
if (resol>big_size)
    {
        resol=big_size;
        LWARNING("Resolution > Cell size!");
    }

if (dr>resol/2)
    {
        dr=resol/2;
        LWARNING("Grid step > resolution!");
    }


float TempVoxel=big_size/dr;
float r=log10(TempVoxel)/log10(2);
int rr=ceil(r);

int NewL=pow(2,rr);
float powN=pow(NN,3);
float powNweL=pow(NewL,3);
NumberVoxel_Structure=NewL;
dr=big_size/NewL; //step of grid
float scale=dr;
float V=pow(big_size,3);

std::cout << "NumberVoxels: "<<NumberVoxel_Structure<<std::endl;
std::cout << "Reflection number: "<<NN<<std::endl;
std::cout << "Resolution: "<< resol<<std::endl;


VolumeRAM* realData = new VolumeAtomic<float>(ivec3(NumberVoxel_Structure,NumberVoxel_Structure,NumberVoxel_Structure));
VolumeRAM* complexData = new VolumeAtomic<float>(ivec3(NumberVoxel_Structure,NumberVoxel_Structure,NumberVoxel_Structure));
VolumeRAM* targetDataset = new VolumeAtomic<float>(ivec3(NumberVoxel_Structure,NumberVoxel_Structure,NumberVoxel_Structure));

for (int hh=0; hh<NewL; hh++)
for (int kk=0; kk<NewL; kk++)
for (int ll=0; ll<NewL; ll++)
{
((VolumeAtomic<float>*)realData)->voxel(hh,kk,ll)=0;
((VolumeAtomic<float>*)complexData)->voxel(hh,kk,ll)=0;
((VolumeAtomic<float>*)targetDataset)->voxel(hh,kk,ll)=0.0;
}


//-----------------------------------------
//---------calc structure factors----------
//-----------------------------------------
std::string src,dst;
int pos;
float atomx,atomy,atomz;
for (int hh=0; hh<NN; hh++)
for (int kk=0; kk<NN; kk++)
for (int ll=0; ll<NN; ll++)
{

float tempreal=0,tempcomplex=0;
for (int i = 1; i <= mol.NumAtoms(); i++)
    {
        OBAtom* a = mol.GetAtom(i);
        src=a->GetType();
        ttab.SetFromType("INT");
        ttab.SetToType("XYZ");
        ttab.Translate(dst,src);
        int p=0;
        while (dst!=sAtomED.AtomName[p]) p=p+1;
        atomx=(a->x()-cx+big_size/2)/big_size;
        atomy=(a->y()-cy+big_size/2)/big_size;
        atomz=(a->z()-cz+big_size/2)/big_size;
        //scattering
        ddd=big_size/sqrt(pow(hh-NN,2)+pow(kk-NN,2)+pow(ll-NN,2)+0.0001);
        float scat=sAtomED.a1[p]*exp(-sAtomED.b1[p]*pow(1/(2*ddd),2))
    +sAtomED.a2[p]*exp(-sAtomED.b2[p]*pow(1/(2*ddd),2))
	+sAtomED.a3[p]*exp(-sAtomED.b3[p]*pow(1/(2*ddd),2))
	+sAtomED.a4[p]*exp(-sAtomED.b4[p]*pow(1/(2*ddd),2))+sAtomED.c[p];
        //----------
        tempreal=tempreal+scat*cos(2*PI*((hh-NN)*atomx+(kk-NN)*atomy+(ll-NN)*atomz));
        tempcomplex=tempcomplex+scat*sin(2*PI*((hh-NN)*atomx+(kk-NN)*atomy+(ll-NN)*atomz));

    }


((VolumeAtomic<float>*)realData)->voxel(hh,kk,ll)=tempreal;
((VolumeAtomic<float>*)complexData)->voxel(hh,kk,ll)=tempcomplex;
std::cout << "Calculate structure factors...: "<<(hh+1)*100/(NN)<< "%"<<"\r";
}

std::cout << "Finish calc structure factor!"<<std::endl;
//-----------------------------------------
//-----------------------------------------

std::cout << "FFT..."<<std::endl;
for (int hh=0; hh<NewL; hh++)
for (int kk=0; kk<NewL; kk++)
for (int ll=0; ll<NewL; ll++)
{
    pos =ll+kk*NewL+hh*NewL*NewL;
    StF[pos].Im=((VolumeAtomic<float>*)complexData)->voxel(hh,kk,ll);
    StF[pos].Re=((VolumeAtomic<float>*)realData)->voxel(hh,kk,ll);
}


fft3D(StF, NewL,NewL,NewL, -1);

//-----------------------------------------
//----------create out volume--------------
//-----------------------------------------
std::cout << "Generate out volume..."<<std::endl;
for (int i=0; i<NumberVoxel_Structure; i++)
for (int j=0; j<NumberVoxel_Structure; j++)
for (int k=0; k<NumberVoxel_Structure; k++)
{
    pos=k+j*NumberVoxel_Structure+i*NumberVoxel_Structure*NumberVoxel_Structure;
float ed=sqrt(pow(StF[pos].Re,2)+pow(StF[pos].Im,2));
((VolumeAtomic<float>*)targetDataset)->voxel(NumberVoxel_Structure-1-i,NumberVoxel_Structure-1-j,NumberVoxel_Structure-1-k)=ed/V;



}


//-----------------------------------------
//-----------------------------------------

if (calcelectronnumb_.get()==true) CalcElectronNumber(targetDataset,NumberOfElectrons);

float ca=cos(PI);
float sa=sin(PI);

MoleculeVolume* volumeHandle = new MoleculeVolume(
            targetDataset,                                                                                // data
            vec3(scale,scale,scale),                                                                      // scale
            vec3(-(big_size/2)+cx+dr/2,-(big_size/2)+cy+dr/2,-(big_size/2)+cz+dr/2), // offset
            InputMoll->getTransformationMatrix()                                                                                     // transform
        );
        
volumeHandle->setMolecule(InputMoll);


return volumeHandle;


//-----------------------------------------
//-----------------------------------------
}


//-----------------------------------------
//--------Calc number of electron----------
//-----------------------------------------
void PDBtoEDM::CalcElectronNumber(VolumeRAM* targetDataset, float ElectronNumber)
{
float ENumber=0;
float temp,delta;
ivec3 NN=((VolumeAtomic<float>*)targetDataset)->getDimensions();
for (int i=0; i<NN[0]; i++)
for (int j=0; j<NN[1]; j++)
for (int k=0; k<NN[2]; k++)
{
ENumber=ENumber+((VolumeAtomic<float>*)targetDataset)->voxel(i,j,k)*dr*dr*dr;
}
std::cout << "Normalisation electron number...: "<<std::endl;
delta=ElectronNumber/ENumber;
for (int i=0; i<NN[0]; i++)
for (int j=0; j<NN[1]; j++)
for (int k=0; k<NN[2]; k++)
{
((VolumeAtomic<float>*)targetDataset)->voxel(i,j,k)=((VolumeAtomic<float>*)targetDataset)->voxel(i,j,k)*delta;
}

std::cout << "Normalization finish:"<<std::endl;
std::cout << "Old:"<<ENumber<<std::endl;
std::cout << "New:"<<ElectronNumber<<std::endl;
}
//-----------------------------------------
//-----------------------------------------


//-----------------------------------------
//--------find atom types in pdb-----------
//-----------------------------------------
void PDBtoEDM::FindAtomTypesInPDB(const OBMol mol, struct AtomicED* sAtomED)
{
NumberAtom=0;
cx=0;
cy=0;
cz=0;
NumberOfElectrons=0;
std::string src,dst;
    for (int i = 1; i <= mol.NumAtoms(); i++)
    {
        OBAtom* a = mol.GetAtom(i);
        src=a->GetType();
        NumberOfElectrons=NumberOfElectrons+a->GetAtomicNum();
        float atomx=a->x();
        float atomy=a->y();
        float atomz=a->z();
        cx=cx+atomx;
        cy=cy+atomy;
        cz=cz+atomz;

        ttab.SetFromType("INT");
        ttab.SetToType("XYZ");
        ttab.Translate(dst,src);
        int Flag=1;

         for (int k = 0; k<NumberAtom;  k++)
           if (dst==sAtomED->AtomName[k]) Flag=0;
           if (Flag!=0)
           {
               sAtomED->AtomName[NumberAtom]=dst;
               NumberAtom=NumberAtom+1;
           }

    }
cx=cx/mol.NumAtoms();
cy=cy/mol.NumAtoms();
cz=cz/mol.NumAtoms();
sAtomED->NumberTypes=NumberAtom;
std::cout << "Number of atoms in input PDB: "<< mol.NumAtoms() << std::endl;
std::cout << "Number of atom types in input PDB: "<< NumberAtom << std::endl;
std::cout << "Center: ["<< cx << ";"<< cy << ";"<<cz<<"]"<< std::endl;

int AFconst=213;
std::string element[AFconst];
float a1[AFconst],b1[AFconst],a2[AFconst],b2[AFconst],a3[AFconst],b3[AFconst],a4[AFconst],b4[AFconst],c[AFconst];
element[0]="HT"; a1[1]=0.493002; b1[1]=10.5109; a2[1]=0.322912; b2[1]=26.1257; a3[1]=0.140191; b3[1]=3.14236; a4[1]=0.04081; b4[1]=57.7997; c[1]=0.003038;
element[1]="H"; a1[1]=0.493002; b1[1]=10.5109; a2[1]=0.322912; b2[1]=26.1257; a3[1]=0.140191; b3[1]=3.14236; a4[1]=0.04081; b4[1]=57.7997; c[1]=0.003038;
element[2]="H"; a1[2]=0.489918; b1[2]=20.6593; a2[2]=0.262003; b2[2]=7.74039; a3[2]=0.196767; b3[2]=49.5519; a4[2]=0.049879; b4[2]=2.20159; c[2]=0.001305;
element[3]="H1-"; a1[3]=0.897661; b1[3]=53.1368; a2[3]=0.565616; b2[3]=15.187; a3[3]=0.415815; b3[3]=186.576; a4[3]=0.116973; b4[3]=3.56709; c[3]=0.002389;
element[4]="He"; a1[4]=0.8734; b1[4]=9.1037; a2[4]=0.6309; b2[4]=3.3568; a3[4]=0.3112; b3[4]=22.9276; a4[4]=0.178; b4[4]=0.9821; c[4]=0.0064;
element[5]="Li"; a1[5]=1.1282; b1[5]=3.9546; a2[5]=0.7508; b2[5]=1.0524; a3[5]=0.6175; b3[5]=85.3905; a4[5]=0.4653; b4[5]=168.261; c[5]=0.0377;
element[6]="Li1+"; a1[6]=0.6968; b1[6]=4.6237; a2[6]=0.7888; b2[6]=1.9557; a3[6]=0.3414; b3[6]=0.6316; a4[6]=0.1563; b4[6]=10.0953; c[6]=0.0167;
element[7]="Be"; a1[7]=1.5919; b1[7]=43.6427; a2[7]=1.1278; b2[7]=1.8623; a3[7]=0.5391; b3[7]=103.483; a4[7]=0.7029; b4[7]=0.542; c[7]=0.0385;
element[8]="Be2+"; a1[8]=6.2603; b1[8]=0.0027; a2[8]=0.8849; b2[8]=0.8313; a3[8]=0.7993; b3[8]=2.2758; a4[8]=0.1647; b4[8]=5.1146; c[8]=-6.1092;
element[9]="B"; a1[9]=2.0545; b1[9]=23.2185; a2[9]=1.3326; b2[9]=1.021; a3[9]=1.0979; b3[9]=60.3498; a4[9]=0.7068; b4[9]=0.1403; c[9]=-0.19320;
element[10]="C"; a1[10]=2.31; b1[10]=20.8439; a2[10]=1.02; b2[10]=10.2075; a3[10]=1.5886; b3[10]=0.5687; a4[10]=0.865; b4[10]=51.6512; c[10]=0.2156;
element[11]="Cval"; a1[11]=2.26069; b1[11]=22.6907; a2[11]=1.56165; b2[11]=0.656665; a3[11]=1.05075; b3[11]=9.75618; a4[11]=0.839259; b4[11]=55.5949; c[11]=0.286977;
element[12]="N"; a1[12]=12.2126; b1[12]=0.0057; a2[12]=3.1322; b2[12]=9.8933; a3[12]=2.0125; b3[12]=28.9975; a4[12]=1.1663; b4[12]=0.5826; c[12]=-11.529;
element[13]="O"; a1[13]=3.0485; b1[13]=13.2771; a2[13]=2.2868; b2[13]=5.7011; a3[13]=1.5463; b3[13]=0.3239; a4[13]=0.867; b4[13]=32.9089; c[13]=0.2508;
element[14]="O1-"; a1[14]=4.1916; b1[14]=12.8573; a2[14]=1.63969; b2[14]=4.17236; a3[14]=1.52673; b3[14]=47.0179; a4[14]=-20.307; b4[14]=-0.01404; c[14]=21.9412;
element[15]="F"; a1[15]=3.5392; b1[15]=10.2825; a2[15]=2.6412; b2[15]=4.2944; a3[15]=1.517; b3[15]=0.2615; a4[15]=1.0243; b4[15]=26.1476; c[15]=0.2776;
element[16]="F1-"; a1[16]=3.6322; b1[16]=5.27756; a2[16]=3.51057; b2[16]=14.7353; a3[16]=1.26064; b3[16]=0.442258; a4[16]=0.940706; b4[16]=47.3437; c[16]=0.653396;
element[17]="Ne"; a1[17]=3.9553; b1[17]=8.4042; a2[17]=3.1125; b2[17]=3.4262; a3[17]=1.4546; b3[17]=0.2306; a4[17]=1.1251; b4[17]=21.7184; c[17]=0.3515;
element[18]="Na"; a1[18]=4.7626; b1[18]=3.285; a2[18]=3.1736; b2[18]=8.8422; a3[18]=1.2674; b3[18]=0.3136; a4[18]=1.1128; b4[18]=129.424; c[18]=0.676;
element[19]="Na1+"; a1[19]=3.2565; b1[19]=2.6671; a2[19]=3.9362; b2[19]=6.1153; a3[19]=1.3998; b3[19]=0.2001; a4[19]=1.0032; b4[19]=14.039; c[19]=0.404;
element[20]="Mg"; a1[20]=5.4204; b1[20]=2.8275; a2[20]=2.1735; b2[20]=79.2611; a3[20]=1.2269; b3[20]=0.3808; a4[20]=2.3073; b4[20]=7.1937; c[20]=0.8584;
element[21]="Mg2+"; a1[21]=3.4988; b1[21]=2.1676; a2[21]=3.8378; b2[21]=4.7542; a3[21]=1.3284; b3[21]=0.185; a4[21]=0.8497; b4[21]=10.1411; c[21]=0.4853;
element[22]="Al"; a1[22]=6.4202; b1[22]=3.0387; a2[22]=1.9002; b2[22]=0.7426; a3[22]=1.5936; b3[22]=31.5472; a4[22]=1.9646; b4[22]=85.0886; c[22]=1.1151;
element[23]="Al3+"; a1[23]=4.17448; b1[23]=1.93816; a2[23]=3.3876; b2[23]=4.14553; a3[23]=1.20296; b3[23]=0.228753; a4[23]=0.528137; b4[23]=8.28524; c[23]=0.706786;
element[24]="Siv"; a1[24]=6.2915; b1[24]=2.4386; a2[24]=3.0353; b2[24]=32.3337; a3[24]=1.9891; b3[24]=0.6785; a4[24]=1.541; b4[24]=81.6937; c[24]=1.1407;
element[25]="Sival"; a1[25]=5.66269; b1[25]=2.6652; a2[25]=3.07164; b2[25]=38.6634; a3[25]=2.62446; b3[25]=0.916946; a4[25]=1.3932; b4[25]=93.5458; c[25]=1.24707;
element[26]="Si4+"; a1[26]=4.43918; b1[26]=1.64167; a2[26]=3.20345; b2[26]=3.43757; a3[26]=1.19453; b3[26]=0.2149; a4[26]=0.41653; b4[26]=6.65365; c[26]=0.746297;
element[27]="P"; a1[27]=6.4345; b1[27]=1.9067; a2[27]=4.1791; b2[27]=27.157; a3[27]=1.78; b3[27]=0.526; a4[27]=1.4908; b4[27]=68.1645; c[27]=1.1149;
element[28]="S"; a1[28]=6.9053; b1[28]=1.4679; a2[28]=5.2034; b2[28]=22.2151; a3[28]=1.4379; b3[28]=0.2536; a4[28]=1.5863; b4[28]=56.172; c[28]=0.8669;
element[29]="Cl"; a1[29]=11.4604; b1[29]=0.0104; a2[29]=7.1964; b2[29]=1.1662; a3[29]=6.2556; b3[29]=18.5194; a4[29]=1.6455; b4[29]=47.7784; c[29]=-9.5574;
element[30]="Cl1-"; a1[30]=18.2915; b1[30]=0.0066; a2[30]=7.2084; b2[30]=1.1717; a3[30]=6.5337; b3[30]=19.5424; a4[30]=2.3386; b4[30]=60.4486; c[30]=-16.378;
element[31]="Ar"; a1[31]=7.4845; b1[31]=0.9072; a2[31]=6.7723; b2[31]=14.8407; a3[31]=0.6539; b3[31]=43.8983; a4[31]=1.6442; b4[31]=33.3929; c[31]=1.4445;
element[32]="K"; a1[32]=8.2186; b1[32]=12.7949; a2[32]=7.4398; b2[32]=0.7748; a3[32]=1.0519; b3[32]=213.187; a4[32]=0.8659; b4[32]=41.6841; c[32]=1.4228;
element[33]="K1+"; a1[33]=7.9578; b1[33]=12.6331; a2[33]=7.4917; b2[33]=0.7674; a3[33]=6.359; b3[33]=-0.00200; a4[33]=1.1915; b4[33]=31.9128; c[33]=-4.9978;
element[34]="Ca"; a1[34]=8.6266; b1[34]=10.4421; a2[34]=7.3873; b2[34]=0.6599; a3[34]=1.5899; b3[34]=85.7484; a4[34]=1.0211; b4[34]=178.437; c[34]=1.3751;
element[35]="Ca2+"; a1[35]=15.6348; b1[35]=-0.00740; a2[35]=7.9518; b2[35]=0.6089; a3[35]=8.4372; b3[35]=10.3116; a4[35]=0.8537; b4[35]=25.9905; c[35]=-14.875;
element[36]="Sc"; a1[36]=9.189; b1[36]=9.0213; a2[36]=7.3679; b2[36]=0.5729; a3[36]=1.6409; b3[36]=136.108; a4[36]=1.468; b4[36]=51.3531; c[36]=1.3329;
element[37]="Sc3+"; a1[37]=13.4008; b1[37]=0.29854; a2[37]=8.0273; b2[37]=7.9629; a3[37]=1.65943; b3[37]=-0.28604; a4[37]=1.57936; b4[37]=16.0662; c[37]=-6.6667;
element[38]="Ti"; a1[38]=9.7595; b1[38]=7.8508; a2[38]=7.3558; b2[38]=0.5; a3[38]=1.6991; b3[38]=35.6338; a4[38]=1.9021; b4[38]=116.105; c[38]=1.2807;
element[39]="Ti2+"; a1[39]=9.11423; b1[39]=7.5243; a2[39]=7.62174; b2[39]=0.457585; a3[39]=2.2793; b3[39]=19.5361; a4[39]=0.087899; b4[39]=61.6558; c[39]=0.897155;
element[40]="Ti3+"; a1[40]=17.7344; b1[40]=0.22061; a2[40]=8.73816; b2[40]=7.04716; a3[40]=5.25691; b3[40]=-0.15762; a4[40]=1.92134; b4[40]=15.9768; c[40]=-14.652;
element[41]="Ti4+"; a1[41]=19.5114; b1[41]=0.178847; a2[41]=8.23473; b2[41]=6.67018; a3[41]=2.01341; b3[41]=-0.29263; a4[41]=1.5208; b4[41]=12.9464; c[41]=-13.280;
element[42]="V"; a1[42]=10.2971; b1[42]=6.8657; a2[42]=7.3511; b2[42]=0.4385; a3[42]=2.0703; b3[42]=26.8938; a4[42]=2.0571; b4[42]=102.478; c[42]=1.2199;
element[43]="V2+"; a1[43]=10.106; b1[43]=6.8818; a2[43]=7.3541; b2[43]=0.4409; a3[43]=2.2884; b3[43]=20.3004; a4[43]=0.0223; b4[43]=115.122; c[43]=1.2298;
element[44]="V3+"; a1[44]=9.43141; b1[44]=6.39535; a2[44]=7.7419; b2[44]=0.383349; a3[44]=2.15343; b3[44]=15.1908; a4[44]=0.016865; b4[44]=63.969; c[44]=0.656565;
element[45]="V5+"; a1[45]=15.6887; b1[45]=0.679003; a2[45]=8.14208; b2[45]=5.40135; a3[45]=2.03081; b3[45]=9.97278; a4[45]=-9.5760; b4[45]=0.940464; c[45]=1.7143;
element[46]="Cr"; a1[46]=10.6406; b1[46]=6.1038; a2[46]=7.3537; b2[46]=0.392; a3[46]=3.324; b3[46]=20.2626; a4[46]=1.4922; b4[46]=98.7399; c[46]=1.1832;
element[47]="Cr2+"; a1[47]=9.54034; b1[47]=5.66078; a2[47]=7.7509; b2[47]=0.344261; a3[47]=3.58274; b3[47]=13.3075; a4[47]=0.509107; b4[47]=32.4224; c[47]=0.616898;
element[48]="Cr3+"; a1[48]=9.6809; b1[48]=5.59463; a2[48]=7.81136; b2[48]=0.334393; a3[48]=2.87603; b3[48]=12.8288; a4[48]=0.113575; b4[48]=32.8761; c[48]=0.518275;
element[49]="Mn"; a1[49]=11.2819; b1[49]=5.3409; a2[49]=7.3573; b2[49]=0.3432; a3[49]=3.0193; b3[49]=17.8674; a4[49]=2.2441; b4[49]=83.7543; c[49]=1.0896;
element[50]="Mn2+"; a1[50]=10.8061; b1[50]=5.2796; a2[50]=7.362; b2[50]=0.3435; a3[50]=3.5268; b3[50]=14.343; a4[50]=0.2184; b4[50]=41.3235; c[50]=1.0874;
element[51]="Mn3+"; a1[51]=9.84521; b1[51]=4.91797; a2[51]=7.87194; b2[51]=0.294393; a3[51]=3.56531; b3[51]=10.8171; a4[51]=0.323613; b4[51]=24.1281; c[51]=0.393974;
element[52]="Mn4+"; a1[52]=9.96253; b1[52]=4.8485; a2[52]=7.97057; b2[52]=0.283303; a3[52]=2.76067; b3[52]=10.4852; a4[52]=0.054447; b4[52]=27.573; c[52]=0.251877;
element[53]="Fe"; a1[53]=11.7695; b1[53]=4.7611; a2[53]=7.3573; b2[53]=0.3072; a3[53]=3.5222; b3[53]=15.3535; a4[53]=2.3045; b4[53]=76.8805; c[53]=1.0369;
element[54]="Fe2+"; a1[54]=11.0424; b1[54]=4.6538; a2[54]=7.374; b2[54]=0.3053; a3[54]=4.1346; b3[54]=12.0546; a4[54]=0.4399; b4[54]=31.2809; c[54]=1.0097;
element[55]="Fe3+"; a1[55]=11.1764; b1[55]=4.6147; a2[55]=7.3863; b2[55]=0.3005; a3[55]=3.3948; b3[55]=11.6729; a4[55]=0.0724; b4[55]=38.5566; c[55]=0.9707;
element[56]="Co"; a1[56]=12.2841; b1[56]=4.2791; a2[56]=7.3409; b2[56]=0.2784; a3[56]=4.0034; b3[56]=13.5359; a4[56]=2.3488; b4[56]=71.1692; c[56]=1.0118;
element[57]="Co2+"; a1[57]=11.2296; b1[57]=4.1231; a2[57]=7.3883; b2[57]=0.2726; a3[57]=4.7393; b3[57]=10.2443; a4[57]=0.7108; b4[57]=25.6466; c[57]=0.9324;
element[58]="Co3+"; a1[58]=10.338; b1[58]=3.90969; a2[58]=7.88173; b2[58]=0.238668; a3[58]=4.76795; b3[58]=8.35583; a4[58]=0.725591; b4[58]=18.3491; c[58]=0.286667;
element[59]="Ni"; a1[59]=12.8376; b1[59]=3.8785; a2[59]=7.292; b2[59]=0.2565; a3[59]=4.4438; b3[59]=12.1763; a4[59]=2.38; b4[59]=66.3421; c[59]=1.0341;
element[60]="Ni2+"; a1[60]=11.4166; b1[60]=3.6766; a2[60]=7.4005; b2[60]=0.2449; a3[60]=5.3442; b3[60]=8.873; a4[60]=0.9773; b4[60]=22.1626; c[60]=0.8614;
element[61]="Ni3+"; a1[61]=10.7806; b1[61]=3.5477; a2[61]=7.75868; b2[61]=0.22314; a3[61]=5.22746; b3[61]=7.64468; a4[61]=0.847114; b4[61]=16.9673; c[61]=0.386044;
element[62]="Cu"; a1[62]=13.338; b1[62]=3.5828; a2[62]=7.1676; b2[62]=0.247; a3[62]=5.6158; b3[62]=11.3966; a4[62]=1.6735; b4[62]=64.8126; c[62]=1.191;
element[63]="Cu1+"; a1[63]=11.9475; b1[63]=3.3669; a2[63]=7.3573; b2[63]=0.2274; a3[63]=6.2455; b3[63]=8.6625; a4[63]=1.5578; b4[63]=25.8487; c[63]=0.89;
element[64]="Cu2+"; a1[64]=11.8168; b1[64]=3.37484; a2[64]=7.11181; b2[64]=0.244078; a3[64]=5.78135; b3[64]=7.9876; a4[64]=1.14523; b4[64]=19.897; c[64]=1.14431;
element[65]="Zn"; a1[65]=14.0743; b1[65]=3.2655; a2[65]=7.0318; b2[65]=0.2333; a3[65]=5.1652; b3[65]=10.3163; a4[65]=2.41; b4[65]=58.7097; c[65]=1.3041;
element[66]="Zn2+"; a1[66]=11.9719; b1[66]=2.9946; a2[66]=7.3862; b2[66]=0.2031; a3[66]=6.4668; b3[66]=7.0826; a4[66]=1.394; b4[66]=18.0995; c[66]=0.7807;
element[67]="Ga"; a1[67]=15.2354; b1[67]=3.0669; a2[67]=6.7006; b2[67]=0.2412; a3[67]=4.3591; b3[67]=10.7805; a4[67]=2.9623; b4[67]=61.4135; c[67]=1.7189;
element[68]="Ga3+"; a1[68]=12.692; b1[68]=2.81262; a2[68]=6.69883; b2[68]=0.22789; a3[68]=6.06692; b3[68]=6.36441; a4[68]=1.0066; b4[68]=14.4122; c[68]=1.53545;
element[69]="Ge"; a1[69]=16.0816; b1[69]=2.8509; a2[69]=6.3747; b2[69]=0.2516; a3[69]=3.7068; b3[69]=11.4468; a4[69]=3.683; b4[69]=54.7625; c[69]=2.1313;
element[70]="Ge4+"; a1[70]=12.9172; b1[70]=2.53718; a2[70]=6.70003; b2[70]=0.205855; a3[70]=6.06791; b3[70]=5.47913; a4[70]=0.859041; b4[70]=11.603; c[70]=1.45572;
element[71]="As"; a1[71]=16.6723; b1[71]=2.6345; a2[71]=6.0701; b2[71]=0.2647; a3[71]=3.4313; b3[71]=12.9479; a4[71]=4.2779; b4[71]=47.7972; c[71]=2.531;
element[72]="Se"; a1[72]=17.0006; b1[72]=2.4098; a2[72]=5.8196; b2[72]=0.2726; a3[72]=3.9731; b3[72]=15.2372; a4[72]=4.3543; b4[72]=43.8163; c[72]=2.8409;
element[73]="Br"; a1[73]=17.1789; b1[73]=2.1723; a2[73]=5.2358; b2[73]=16.5796; a3[73]=5.6377; b3[73]=0.2609; a4[73]=3.9851; b4[73]=41.4328; c[73]=2.9557;
element[74]="Br1-"; a1[74]=17.1718; b1[74]=2.2059; a2[74]=6.3338; b2[74]=19.3345; a3[74]=5.5754; b3[74]=0.2871; a4[74]=3.7272; b4[74]=58.1535; c[74]=3.1776;
element[75]="Kr"; a1[75]=17.3555; b1[75]=1.9384; a2[75]=6.7286; b2[75]=16.5623; a3[75]=5.5493; b3[75]=0.2261; a4[75]=3.5375; b4[75]=39.3972; c[75]=2.825;
element[76]="Rb"; a1[76]=17.1784; b1[76]=1.7888; a2[76]=9.6435; b2[76]=17.3151; a3[76]=5.1399; b3[76]=0.2748; a4[76]=1.5292; b4[76]=164.934; c[76]=3.4873;
element[77]="Rb1+"; a1[77]=17.5816; b1[77]=1.7139; a2[77]=7.6598; b2[77]=14.7957; a3[77]=5.8981; b3[77]=0.1603; a4[77]=2.7817; b4[77]=31.2087; c[77]=2.0782;
element[78]="Sr"; a1[78]=17.5663; b1[78]=1.5564; a2[78]=9.8184; b2[78]=14.0988; a3[78]=5.422; b3[78]=0.1664; a4[78]=2.6694; b4[78]=132.376; c[78]=2.5064;
element[79]="Sr2+"; a1[79]=18.0874; b1[79]=1.4907; a2[79]=8.1373; b2[79]=12.6963; a3[79]=2.5654; b3[79]=24.5651; a4[79]=-34.193; b4[79]=-0.01380; c[79]=41.4025;
element[80]="Y"; a1[80]=17.776; b1[80]=1.4029; a2[80]=10.2946; b2[80]=12.8006; a3[80]=5.72629; b3[80]=0.125599; a4[80]=3.26588; b4[80]=104.354; c[80]=1.91213;
element[81]="Y3+"; a1[81]=17.9268; b1[81]=1.35417; a2[81]=9.1531; b2[81]=11.2145; a3[81]=1.76795; b3[81]=22.6599; a4[81]=-33.108; b4[81]=-0.01319; c[81]=40.2602;
element[82]="Zr"; a1[82]=17.8765; b1[82]=1.27618; a2[82]=10.948; b2[82]=11.916; a3[82]=5.41732; b3[82]=0.117622; a4[82]=3.65721; b4[82]=87.6627; c[82]=2.06929;
element[83]="Zr4+"; a1[83]=18.1668; b1[83]=1.2148; a2[83]=10.0562; b2[83]=10.1483; a3[83]=1.01118; b3[83]=21.6054; a4[83]=-2.6479; b4[83]=-0.10276; c[83]=9.41454;
element[84]="Nb"; a1[84]=17.6142; b1[84]=1.18865; a2[84]=12.0144; b2[84]=11.766; a3[84]=4.04183; b3[84]=0.204785; a4[84]=3.53346; b4[84]=69.7957; c[84]=3.75591;
element[85]="Nb3+"; a1[85]=19.8812; b1[85]=0.019175; a2[85]=18.0653; b2[85]=1.13305; a3[85]=11.0177; b3[85]=10.1621; a4[85]=1.94715; b4[85]=28.3389; c[85]=-12.912;
element[86]="Nb5+"; a1[86]=17.9163; b1[86]=1.12446; a2[86]=13.3417; b2[86]=0.028781; a3[86]=10.799; b3[86]=9.28206; a4[86]=0.337905; b4[86]=25.7228; c[86]=-6.3934;
element[87]="Mo"; a1[87]=3.7025; b1[87]=0.2772; a2[87]=17.2356; b2[87]=1.0958; a3[87]=12.8876; b3[87]=11.004; a4[87]=3.7429; b4[87]=61.6584; c[87]=4.3875;
element[88]="Mo3+"; a1[88]=21.1664; b1[88]=0.014734; a2[88]=18.2017; b2[88]=1.03031; a3[88]=11.7423; b3[88]=9.53659; a4[88]=2.30951; b4[88]=26.6307; c[88]=-14.421;
element[89]="Mo5+"; a1[89]=21.0149; b1[89]=0.014345; a2[89]=18.0992; b2[89]=1.02238; a3[89]=11.4632; b3[89]=8.78809; a4[89]=0.740625; b4[89]=23.3452; c[89]=-14.316;
element[90]="Mo6+"; a1[90]=17.8871; b1[90]=1.03649; a2[90]=11.175; b2[90]=8.48061; a3[90]=6.57891; b3[90]=0.058881; a4[90]=0; b4[90]=0; c[90]=0.344941;
element[91]="Tc"; a1[91]=19.1301; b1[91]=0.864132; a2[91]=11.0948; b2[91]=8.14487; a3[91]=4.64901; b3[91]=21.5707; a4[91]=2.71263; b4[91]=86.8472; c[91]=5.40428;
element[92]="Ru"; a1[92]=19.2674; b1[92]=0.80852; a2[92]=12.9182; b2[92]=8.43467; a3[92]=4.86337; b3[92]=24.7997; a4[92]=1.56756; b4[92]=94.2928; c[92]=5.37874;
element[93]="Ru3+"; a1[93]=18.5638; b1[93]=0.847329; a2[93]=13.2885; b2[93]=8.37164; a3[93]=9.32602; b3[93]=0.017662; a4[93]=3.00964; b4[93]=22.887; c[93]=-3.1892;
element[94]="Ru4+"; a1[94]=18.5003; b1[94]=0.844582; a2[94]=13.1787; b2[94]=8.12534; a3[94]=4.71304; b3[94]=0.36495; a4[94]=2.18535; b4[94]=20.8504; c[94]=1.42357;
element[95]="Rh"; a1[95]=19.2957; b1[95]=0.751536; a2[95]=14.3501; b2[95]=8.21758; a3[95]=4.73425; b3[95]=25.8749; a4[95]=1.28918; b4[95]=98.6062; c[95]=5.328;
element[96]="Rh3+"; a1[96]=18.8785; b1[96]=0.764252; a2[96]=14.1259; b2[96]=7.84438; a3[96]=3.32515; b3[96]=21.2487; a4[96]=-6.1989; b4[96]=-0.01036; c[96]=11.8678;
element[97]="Rh4+"; a1[97]=18.8545; b1[97]=0.760825; a2[97]=13.9806; b2[97]=7.62436; a3[97]=2.53464; b3[97]=19.3317; a4[97]=-5.6526; b4[97]=-0.01020; c[97]=11.2835;
element[98]="Pd"; a1[98]=19.3319; b1[98]=0.698655; a2[98]=15.5017; b2[98]=7.98929; a3[98]=5.29537; b3[98]=25.2052; a4[98]=0.605844; b4[98]=76.8986; c[98]=5.26593;
element[99]="Pd2+"; a1[99]=19.1701; b1[99]=0.696219; a2[99]=15.2096; b2[99]=7.55573; a3[99]=4.32234; b3[99]=22.5057; a4[99]=0; b4[99]=0; c[99]=5.2916;
element[100]="Pd4+"; a1[100]=19.2493; b1[100]=0.683839; a2[100]=14.79; b2[100]=7.14833; a3[100]=2.89289; b3[100]=17.9144; a4[100]=-7.9492; b4[100]=0.005127; c[100]=13.0174;
element[101]="Ag"; a1[101]=19.2808; b1[101]=0.6446; a2[101]=16.6885; b2[101]=7.4726; a3[101]=4.8045; b3[101]=24.6605; a4[101]=1.0463; b4[101]=99.8156; c[101]=5.179;
element[102]="Ag1+"; a1[102]=19.1812; b1[102]=0.646179; a2[102]=15.9719; b2[102]=7.19123; a3[102]=5.27475; b3[102]=21.7326; a4[102]=0.357534; b4[102]=66.1147; c[102]=5.21572;
element[103]="Ag2+"; a1[103]=19.1643; b1[103]=0.645643; a2[103]=16.2456; b2[103]=7.18544; a3[103]=4.3709; b3[103]=21.4072; a4[103]=0; b4[103]=0; c[103]=5.21404;
element[104]="Cd"; a1[104]=19.2214; b1[104]=0.5946; a2[104]=17.6444; b2[104]=6.9089; a3[104]=4.461; b3[104]=24.7008; a4[104]=1.6029; b4[104]=87.4825; c[104]=5.0694;
element[105]="Cd2+"; a1[105]=19.1514; b1[105]=0.597922; a2[105]=17.2535; b2[105]=6.80639; a3[105]=4.47128; b3[105]=20.2521; a4[105]=0; b4[105]=0; c[105]=5.11937;
element[106]="In"; a1[106]=19.1624; b1[106]=0.5476; a2[106]=18.5596; b2[106]=6.3776; a3[106]=4.2948; b3[106]=25.8499; a4[106]=2.0396; b4[106]=92.8029; c[106]=4.9391;
element[107]="In3+"; a1[107]=19.1045; b1[107]=0.551522; a2[107]=18.1108; b2[107]=6.3247; a3[107]=3.78897; b3[107]=17.3595; a4[107]=0; b4[107]=0; c[107]=4.99635;
element[108]="Sn"; a1[108]=19.1889; b1[108]=5.8303; a2[108]=19.1005; b2[108]=0.5031; a3[108]=4.4585; b3[108]=26.8909; a4[108]=2.4663; b4[108]=83.9571; c[108]=4.7821;
element[109]="Sn2+"; a1[109]=19.1094; b1[109]=0.5036; a2[109]=19.0548; b2[109]=5.8378; a3[109]=4.5648; b3[109]=23.3752; a4[109]=0.487; b4[109]=62.2061; c[109]=4.7861;
element[110]="Sn4+"; a1[110]=18.9333; b1[110]=5.764; a2[110]=19.7131; b2[110]=0.4655; a3[110]=3.4182; b3[110]=14.0049; a4[110]=0.0193; b4[110]=-0.75830; c[110]=3.9182;
element[111]="Sb"; a1[111]=19.6418; b1[111]=5.3034; a2[111]=19.0455; b2[111]=0.4607; a3[111]=5.0371; b3[111]=27.9074; a4[111]=2.6827; b4[111]=75.2825; c[111]=4.5909;
element[112]="Sb3+"; a1[112]=18.9755; b1[112]=0.467196; a2[112]=18.933; b2[112]=5.22126; a3[112]=5.10789; b3[112]=19.5902; a4[112]=0.288753; b4[112]=55.5113; c[112]=4.69626;
element[113]="Sb5+"; a1[113]=19.8685; b1[113]=5.44853; a2[113]=19.0302; b2[113]=0.467973; a3[113]=2.41253; b3[113]=14.1259; a4[113]=0; b4[113]=0; c[113]=4.69263;
element[114]="Te"; a1[114]=19.9644; b1[114]=4.81742; a2[114]=19.0138; b2[114]=0.420885; a3[114]=6.14487; b3[114]=28.5284; a4[114]=2.5239; b4[114]=70.8403; c[114]=4.352;
element[115]="I"; a1[115]=20.1472; b1[115]=4.347; a2[115]=18.9949; b2[115]=0.3814; a3[115]=7.5138; b3[115]=27.766; a4[115]=2.2735; b4[115]=66.8776; c[115]=4.0712;
element[116]="I1-"; a1[116]=20.2332; b1[116]=4.3579; a2[116]=18.997; b2[116]=0.3815; a3[116]=7.8069; b3[116]=29.5259; a4[116]=2.8868; b4[116]=84.9304; c[116]=4.0714;
element[117]="Xe"; a1[117]=20.2933; b1[117]=3.9282; a2[117]=19.0298; b2[117]=0.344; a3[117]=8.9767; b3[117]=26.4659; a4[117]=1.99; b4[117]=64.2658; c[117]=3.7118;
element[118]="Cs"; a1[118]=20.3892; b1[118]=3.569; a2[118]=19.1062; b2[118]=0.3107; a3[118]=10.662; b3[118]=24.3879; a4[118]=1.4953; b4[118]=213.904; c[118]=3.3352;
element[119]="Cs1+"; a1[119]=20.3524; b1[119]=3.552; a2[119]=19.1278; b2[119]=0.3086; a3[119]=10.2821; b3[119]=23.7128; a4[119]=0.9615; b4[119]=59.4565; c[119]=3.2791;
element[120]="Ba"; a1[120]=20.3361; b1[120]=3.216; a2[120]=19.297; b2[120]=0.2756; a3[120]=10.888; b3[120]=20.2073; a4[120]=2.6959; b4[120]=167.202; c[120]=2.7731;
element[121]="Ba2+"; a1[121]=20.1807; b1[121]=3.21367; a2[121]=19.1136; b2[121]=0.28331; a3[121]=10.9054; b3[121]=20.0558; a4[121]=0.77634; b4[121]=51.746; c[121]=3.02902;
element[122]="La"; a1[122]=20.578; b1[122]=2.94817; a2[122]=19.599; b2[122]=0.244475; a3[122]=11.3727; b3[122]=18.7726; a4[122]=3.28719; b4[122]=133.124; c[122]=2.14678;
element[123]="La3+"; a1[123]=20.2489; b1[123]=2.9207; a2[123]=19.3763; b2[123]=0.250698; a3[123]=11.6323; b3[123]=17.8211; a4[123]=0.336048; b4[123]=54.9453; c[123]=2.4086;
element[124]="Ce"; a1[124]=21.1671; b1[124]=2.81219; a2[124]=19.7695; b2[124]=0.226836; a3[124]=11.8513; b3[124]=17.6083; a4[124]=3.33049; b4[124]=127.113; c[124]=1.86264;
element[125]="Ce3+"; a1[125]=20.8036; b1[125]=2.77691; a2[125]=19.559; b2[125]=0.23154; a3[125]=11.9369; b3[125]=16.5408; a4[125]=0.612376; b4[125]=43.1692; c[125]=2.09013;
element[126]="Ce4+"; a1[126]=20.3235; b1[126]=2.65941; a2[126]=19.8186; b2[126]=0.21885; a3[126]=12.1233; b3[126]=15.7992; a4[126]=0.144583; b4[126]=62.2355; c[126]=1.5918;
element[127]="Pr"; a1[127]=22.044; b1[127]=2.77393; a2[127]=19.6697; b2[127]=0.222087; a3[127]=12.3856; b3[127]=16.7669; a4[127]=2.82428; b4[127]=143.644; c[127]=2.0583;
element[128]="Pr3+"; a1[128]=21.3727; b1[128]=2.6452; a2[128]=19.7491; b2[128]=0.214299; a3[128]=12.1329; b3[128]=15.323; a4[128]=0.97518; b4[128]=36.4065; c[128]=1.77132;
element[129]="Pr4+"; a1[129]=20.9413; b1[129]=2.54467; a2[129]=20.0539; b2[129]=0.202481; a3[129]=12.4668; b3[129]=14.8137; a4[129]=0.296689; b4[129]=45.4643; c[129]=1.24285;
element[130]="Nd"; a1[130]=22.6845; b1[130]=2.66248; a2[130]=19.6847; b2[130]=0.210628; a3[130]=12.774; b3[130]=15.885; a4[130]=2.85137; b4[130]=137.903; c[130]=1.98486;
element[131]="Nd3+"; a1[131]=21.961; b1[131]=2.52722; a2[131]=19.9339; b2[131]=0.199237; a3[131]=12.12; b3[131]=14.1783; a4[131]=1.51031; b4[131]=30.8717; c[131]=1.47588;
element[132]="Pm"; a1[132]=23.3405; b1[132]=2.5627; a2[132]=19.6095; b2[132]=0.202088; a3[132]=13.1235; b3[132]=15.1009; a4[132]=2.87516; b4[132]=132.721; c[132]=2.02876;
element[133]="Pm3+"; a1[133]=22.5527; b1[133]=2.4174; a2[133]=20.1108; b2[133]=0.185769; a3[133]=12.0671; b3[133]=13.1275; a4[133]=2.07492; b4[133]=27.4491; c[133]=1.19499;
element[134]="Sm"; a1[134]=24.0042; b1[134]=2.47274; a2[134]=19.4258; b2[134]=0.196451; a3[134]=13.4396; b3[134]=14.3996; a4[134]=2.89604; b4[134]=128.007; c[134]=2.20963;
element[135]="Sm3+"; a1[135]=23.1504; b1[135]=2.31641; a2[135]=20.2599; b2[135]=0.174081; a3[135]=11.9202; b3[135]=12.1571; a4[135]=2.71488; b4[135]=24.8242; c[135]=0.954586;
element[136]="Eu"; a1[136]=24.6274; b1[136]=2.3879; a2[136]=19.0886; b2[136]=0.1942; a3[136]=13.7603; b3[136]=13.7546; a4[136]=2.9227; b4[136]=123.174; c[136]=2.5745;
element[137]="Eu2+"; a1[137]=24.0063; b1[137]=2.27783; a2[137]=19.9504; b2[137]=0.17353; a3[137]=11.8034; b3[137]=11.6096; a4[137]=3.87243; b4[137]=26.5156; c[137]=1.36389;
element[138]="Eu3+"; a1[138]=23.7497; b1[138]=2.22258; a2[138]=20.3745; b2[138]=0.16394; a3[138]=11.8509; b3[138]=11.311; a4[138]=3.26503; b4[138]=22.9966; c[138]=0.759344;
element[139]="Gd"; a1[139]=25.0709; b1[139]=2.25341; a2[139]=19.0798; b2[139]=0.181951; a3[139]=13.8518; b3[139]=12.9331; a4[139]=3.54545; b4[139]=101.398; c[139]=2.4196;
element[140]="Gd3+"; a1[140]=24.3466; b1[140]=2.13553; a2[140]=20.4208; b2[140]=0.155525; a3[140]=11.8708; b3[140]=10.5782; a4[140]=3.7149; b4[140]=21.7029; c[140]=0.645089;
element[141]="Tb"; a1[141]=25.8976; b1[141]=2.24256; a2[141]=18.2185; b2[141]=0.196143; a3[141]=14.3167; b3[141]=12.6648; a4[141]=2.95354; b4[141]=115.362; c[141]=3.58324;
element[142]="Tb3+"; a1[142]=24.9559; b1[142]=2.05601; a2[142]=20.3271; b2[142]=0.149525; a3[142]=12.2471; b3[142]=10.0499; a4[142]=3.773; b4[142]=21.2773; c[142]=0.691967;
element[143]="Dy"; a1[143]=26.507; b1[143]=2.1802; a2[143]=17.6383; b2[143]=0.202172; a3[143]=14.5596; b3[143]=12.1899; a4[143]=2.96577; b4[143]=111.874; c[143]=4.29728;
element[144]="Dy3+"; a1[144]=25.5395; b1[144]=1.9804; a2[144]=20.2861; b2[144]=0.143384; a3[144]=11.9812; b3[144]=9.34972; a4[144]=4.50073; b4[144]=19.581; c[144]=0.68969;
element[145]="Ho"; a1[145]=26.9049; b1[145]=2.07051; a2[145]=17.294; b2[145]=0.19794; a3[145]=14.5583; b3[145]=11.4407; a4[145]=3.63837; b4[145]=92.6566; c[145]=4.56796;
element[146]="Ho3+"; a1[146]=26.1296; b1[146]=1.91072; a2[146]=20.0994; b2[146]=0.139358; a3[146]=11.9788; b3[146]=8.80018; a4[146]=4.93676; b4[146]=18.5908; c[146]=0.852795;
element[147]="Er"; a1[147]=27.6563; b1[147]=2.07356; a2[147]=16.4285; b2[147]=0.223545; a3[147]=14.9779; b3[147]=11.3604; a4[147]=2.98233; b4[147]=105.703; c[147]=5.92046;
element[148]="Er3+"; a1[148]=26.722; b1[148]=1.84659; a2[148]=19.7748; b2[148]=0.13729; a3[148]=12.1506; b3[148]=8.36225; a4[148]=5.17379; b4[148]=17.8974; c[148]=1.17613;
element[149]="Tm"; a1[149]=28.1819; b1[149]=2.02859; a2[149]=15.8851; b2[149]=0.238849; a3[149]=15.1542; b3[149]=10.9975; a4[149]=2.98706; b4[149]=102.961; c[149]=6.75621;
element[150]="Tm3+"; a1[150]=27.3083; b1[150]=1.78711; a2[150]=19.332; b2[150]=0.136974; a3[150]=12.3339; b3[150]=7.96778; a4[150]=5.38348; b4[150]=17.2922; c[150]=1.63929;
element[151]="Yb"; a1[151]=28.6641; b1[151]=1.9889; a2[151]=15.4345; b2[151]=0.257119; a3[151]=15.3087; b3[151]=10.6647; a4[151]=2.98963; b4[151]=100.417; c[151]=7.56672;
element[152]="Yb2+"; a1[152]=28.1209; b1[152]=1.78503; a2[152]=17.6817; b2[152]=0.15997; a3[152]=13.3335; b3[152]=8.18304; a4[152]=5.14657; b4[152]=20.39; c[152]=3.70983;
element[153]="Yb3+"; a1[153]=27.8917; b1[153]=1.73272; a2[153]=18.7614; b2[153]=0.13879; a3[153]=12.6072; b3[153]=7.64412; a4[153]=5.47647; b4[153]=16.8153; c[153]=2.26001;
element[154]="Lu"; a1[154]=28.9476; b1[154]=1.90182; a2[154]=15.2208; b2[154]=9.98519; a3[154]=15.1; b3[154]=0.261033; a4[154]=3.71601; b4[154]=84.3298; c[154]=7.97628;
element[155]="Lu3+"; a1[155]=28.4628; b1[155]=1.68216; a2[155]=18.121; b2[155]=0.142292; a3[155]=12.8429; b3[155]=7.33727; a4[155]=5.59415; b4[155]=16.3535; c[155]=2.97573;
element[156]="Hf"; a1[156]=29.144; b1[156]=1.83262; a2[156]=15.1726; b2[156]=9.5999; a3[156]=14.7586; b3[156]=0.275116; a4[156]=4.30013; b4[156]=72.029; c[156]=8.58154;
element[157]="Hf4+"; a1[157]=28.8131; b1[157]=1.59136; a2[157]=18.4601; b2[157]=0.128903; a3[157]=12.7285; b3[157]=6.76232; a4[157]=5.59927; b4[157]=14.0366; c[157]=2.39699;
element[158]="Ta"; a1[158]=29.2024; b1[158]=1.77333; a2[158]=15.2293; b2[158]=9.37046; a3[158]=14.5135; b3[158]=0.295977; a4[158]=4.76492; b4[158]=63.3644; c[158]=9.24354;
element[159]="Ta5+"; a1[159]=29.1587; b1[159]=1.50711; a2[159]=18.8407; b2[159]=0.116741; a3[159]=12.8268; b3[159]=6.31524; a4[159]=5.38695; b4[159]=12.4244; c[159]=1.78555;
element[160]="W"; a1[160]=29.0818; b1[160]=1.72029; a2[160]=15.43; b2[160]=9.2259; a3[160]=14.4327; b3[160]=0.321703; a4[160]=5.11982; b4[160]=57.056; c[160]=9.8875;
element[161]="W6+"; a1[161]=29.4936; b1[161]=1.42755; a2[161]=19.3763; b2[161]=0.104621; a3[161]=13.0544; b3[161]=5.93667; a4[161]=5.06412; b4[161]=11.1972; c[161]=1.01074;
element[162]="Re"; a1[162]=28.7621; b1[162]=1.67191; a2[162]=15.7189; b2[162]=9.09227; a3[162]=14.5564; b3[162]=0.3505; a4[162]=5.44174; b4[162]=52.0861; c[162]=10.472;
element[163]="Os"; a1[163]=28.1894; b1[163]=1.62903; a2[163]=16.155; b2[163]=8.97948; a3[163]=14.9305; b3[163]=0.382661; a4[163]=5.67589; b4[163]=48.1647; c[163]=11.0005;
element[164]="Os4+"; a1[164]=30.419; b1[164]=1.37113; a2[164]=15.2637; b2[164]=6.84706; a3[164]=14.7458; b3[164]=0.165191; a4[164]=5.06795; b4[164]=18.003; c[164]=6.49804;
element[165]="Ir"; a1[165]=27.3049; b1[165]=1.59279; a2[165]=16.7296; b2[165]=8.86553; a3[165]=15.6115; b3[165]=0.417916; a4[165]=5.83377; b4[165]=45.0011; c[165]=11.4722;
element[166]="Ir3+"; a1[166]=30.4156; b1[166]=1.34323; a2[166]=15.862; b2[166]=7.10909; a3[166]=13.6145; b3[166]=0.204633; a4[166]=5.82008; b4[166]=20.3254; c[166]=8.27903;
element[167]="Ir4+"; a1[167]=30.7058; b1[167]=1.30923; a2[167]=15.5512; b2[167]=6.71983; a3[167]=14.2326; b3[167]=0.167252; a4[167]=5.53672; b4[167]=17.4911; c[167]=6.96824;
element[168]="Pt"; a1[168]=27.0059; b1[168]=1.51293; a2[168]=17.7639; b2[168]=8.81174; a3[168]=15.7131; b3[168]=0.424593; a4[168]=5.7837; b4[168]=38.6103; c[168]=11.6883;
element[169]="Pt2+"; a1[169]=29.8429; b1[169]=1.32927; a2[169]=16.7224; b2[169]=7.38979; a3[169]=13.2153; b3[169]=0.263297; a4[169]=6.35234; b4[169]=22.9426; c[169]=9.85329;
element[170]="Pt4+"; a1[170]=30.9612; b1[170]=1.24813; a2[170]=15.9829; b2[170]=6.60834; a3[170]=13.7348; b3[170]=0.16864; a4[170]=5.92034; b4[170]=16.9392; c[170]=7.39534;
element[171]="Au"; a1[171]=16.8819; b1[171]=0.4611; a2[171]=18.5913; b2[171]=8.6216; a3[171]=25.5582; b3[171]=1.4826; a4[171]=5.86; b4[171]=36.3956; c[171]=12.0658;
element[172]="Au1+"; a1[172]=28.0109; b1[172]=1.35321; a2[172]=17.8204; b2[172]=7.7395; a3[172]=14.3359; b3[172]=0.356752; a4[172]=6.58077; b4[172]=26.4043; c[172]=11.2299;
element[173]="Au3+"; a1[173]=30.6886; b1[173]=1.2199; a2[173]=16.9029; b2[173]=6.82872; a3[173]=12.7801; b3[173]=0.212867; a4[173]=6.52354; b4[173]=18.659; c[173]=9.0968;
element[174]="Hg"; a1[174]=20.6809; b1[174]=0.545; a2[174]=19.0417; b2[174]=8.4484; a3[174]=21.6575; b3[174]=1.5729; a4[174]=5.9676; b4[174]=38.3246; c[174]=12.6089;
element[175]="Hg1+"; a1[175]=25.0853; b1[175]=1.39507; a2[175]=18.4973; b2[175]=7.65105; a3[175]=16.8883; b3[175]=0.443378; a4[175]=6.48216; b4[175]=28.2262; c[175]=12.0205;
element[176]="Hg2+"; a1[176]=29.5641; b1[176]=1.21152; a2[176]=18.06; b2[176]=7.05639; a3[176]=12.8374; b3[176]=0.284738; a4[176]=6.89912; b4[176]=20.7482; c[176]=10.6268;
element[177]="Tl"; a1[177]=27.5446; b1[177]=0.65515; a2[177]=19.1584; b2[177]=8.70751; a3[177]=15.538; b3[177]=1.96347; a4[177]=5.52593; b4[177]=45.8149; c[177]=13.1746;
element[178]="Tl1+"; a1[178]=21.3985; b1[178]=1.4711; a2[178]=20.4723; b2[178]=0.517394; a3[178]=18.7478; b3[178]=7.43463; a4[178]=6.82847; b4[178]=28.8482; c[178]=12.5258;
element[179]="Tl3+"; a1[179]=30.8695; b1[179]=1.1008; a2[179]=18.3481; b2[179]=6.53852; a3[179]=11.9328; b3[179]=0.219074; a4[179]=7.00574; b4[179]=17.2114; c[179]=9.8027;
element[180]="Pb"; a1[180]=31.0617; b1[180]=0.6902; a2[180]=13.0637; b2[180]=2.3576; a3[180]=18.442; b3[180]=8.618; a4[180]=5.9696; b4[180]=47.2579; c[180]=13.4118;
element[181]="Pb2+"; a1[181]=21.7886; b1[181]=1.3366; a2[181]=19.5682; b2[181]=0.488383; a3[181]=19.1406; b3[181]=6.7727; a4[181]=7.01107; b4[181]=23.8132; c[181]=12.4734;
element[182]="Pb4+"; a1[182]=32.1244; b1[182]=1.00566; a2[182]=18.8003; b2[182]=6.10926; a3[182]=12.0175; b3[182]=0.147041; a4[182]=6.96886; b4[182]=14.714; c[182]=8.08428;
element[183]="Bi"; a1[183]=33.3689; b1[183]=0.704; a2[183]=12.951; b2[183]=2.9238; a3[183]=16.5877; b3[183]=8.7937; a4[183]=6.4692; b4[183]=48.0093; c[183]=13.5782;
element[184]="Bi3+"; a1[184]=21.8053; b1[184]=1.2356; a2[184]=19.5026; b2[184]=6.24149; a3[184]=19.1053; b3[184]=0.469999; a4[184]=7.10295; b4[184]=20.3185; c[184]=12.4711;
element[185]="Bi5+"; a1[185]=33.5364; b1[185]=0.91654; a2[185]=25.0946; b2[185]=0.39042; a3[185]=19.2497; b3[185]=5.71414; a4[185]=6.91555; b4[185]=12.8285; c[185]=-6.7994;
element[186]="Po"; a1[186]=34.6726; b1[186]=0.700999; a2[186]=15.4733; b2[186]=3.55078; a3[186]=13.1138; b3[186]=9.55642; a4[186]=7.02588; b4[186]=47.0045; c[186]=13.677;
element[187]="At"; a1[187]=35.3163; b1[187]=0.68587; a2[187]=19.0211; b2[187]=3.97458; a3[187]=9.49887; b3[187]=11.3824; a4[187]=7.42518; b4[187]=45.4715; c[187]=13.7108;
element[188]="Rn"; a1[188]=35.5631; b1[188]=0.6631; a2[188]=21.2816; b2[188]=4.0691; a3[188]=8.0037; b3[188]=14.0422; a4[188]=7.4433; b4[188]=44.2473; c[188]=13.6905;
element[189]="Fr"; a1[189]=35.9299; b1[189]=0.646453; a2[189]=23.0547; b2[189]=4.17619; a3[189]=12.1439; b3[189]=23.1052; a4[189]=2.11253; b4[189]=150.645; c[189]=13.7247;
element[190]="Ra"; a1[190]=35.763; b1[190]=0.616341; a2[190]=22.9064; b2[190]=3.87135; a3[190]=12.4739; b3[190]=19.9887; a4[190]=3.21097; b4[190]=142.325; c[190]=13.6211;
element[191]="Ra2+"; a1[191]=35.215; b1[191]=0.604909; a2[191]=21.67; b2[191]=3.5767; a3[191]=7.91342; b3[191]=12.601; a4[191]=7.65078; b4[191]=29.8436; c[191]=13.5431;
element[192]="Ac"; a1[192]=35.6597; b1[192]=0.589092; a2[192]=23.1032; b2[192]=3.65155; a3[192]=12.5977; b3[192]=18.599; a4[192]=4.08655; b4[192]=117.02; c[192]=13.5266;
element[193]="Ac3+"; a1[193]=35.1736; b1[193]=0.579689; a2[193]=22.1112; b2[193]=3.41437; a3[193]=8.19216; b3[193]=12.9187; a4[193]=7.05545; b4[193]=25.9443; c[193]=13.4637;
element[194]="Th"; a1[194]=35.5645; b1[194]=0.563359; a2[194]=23.4219; b2[194]=3.46204; a3[194]=12.7473; b3[194]=17.8309; a4[194]=4.80703; b4[194]=99.1722; c[194]=13.4314;
element[195]="Th4+"; a1[195]=35.1007; b1[195]=0.555054; a2[195]=22.4418; b2[195]=3.24498; a3[195]=9.78554; b3[195]=13.4661; a4[195]=5.29444; b4[195]=23.9533; c[195]=13.376;
element[196]="Pa"; a1[196]=35.8847; b1[196]=0.547751; a2[196]=23.2948; b2[196]=3.41519; a3[196]=14.1891; b3[196]=16.9235; a4[196]=4.17287; b4[196]=105.251; c[196]=13.4287;
element[197]="U"; a1[197]=36.0228; b1[197]=0.5293; a2[197]=23.4128; b2[197]=3.3253; a3[197]=14.9491; b3[197]=16.0927; a4[197]=4.188; b4[197]=100.613; c[197]=13.3966;
element[198]="U3+"; a1[198]=35.5747; b1[198]=0.52048; a2[198]=22.5259; b2[198]=3.12293; a3[198]=12.2165; b3[198]=12.7148; a4[198]=5.37073; b4[198]=26.3394; c[198]=13.3092;
element[199]="U4+"; a1[199]=35.3715; b1[199]=0.516598; a2[199]=22.5326; b2[199]=3.05053; a3[199]=12.0291; b3[199]=12.5723; a4[199]=4.7984; b4[199]=23.4582; c[199]=13.2671;
element[200]="U6+"; a1[200]=34.8509; b1[200]=0.507079; a2[200]=22.7584; b2[200]=2.8903; a3[200]=14.0099; b3[200]=13.1767; a4[200]=1.21457; b4[200]=25.2017; c[200]=13.1665;
element[201]="Np"; a1[201]=36.1874; b1[201]=0.511929; a2[201]=23.5964; b2[201]=3.25396; a3[201]=15.6402; b3[201]=15.3622; a4[201]=4.1855; b4[201]=97.4908; c[201]=13.3573;
element[202]="Np3+"; a1[202]=35.7074; b1[202]=0.502322; a2[202]=22.613; b2[202]=3.03807; a3[202]=12.9898; b3[202]=12.1449; a4[202]=5.43227; b4[202]=25.4928; c[202]=13.2544;
element[203]="Np4+"; a1[203]=35.5103; b1[203]=0.498626; a2[203]=22.5787; b2[203]=2.96627; a3[203]=12.7766; b3[203]=11.9484; a4[203]=4.92159; b4[203]=22.7502; c[203]=13.2116;
element[204]="Np6+"; a1[204]=35.0136; b1[204]=0.48981; a2[204]=22.7286; b2[204]=2.81099; a3[204]=14.3884; b3[204]=12.33; a4[204]=1.75669; b4[204]=22.6581; c[204]=13.113;
element[205]="Pu"; a1[205]=36.5254; b1[205]=0.499384; a2[205]=23.8083; b2[205]=3.26371; a3[205]=16.7707; b3[205]=14.9455; a4[205]=3.47947; b4[205]=105.98; c[205]=13.3812;
element[206]="Pu3+"; a1[206]=35.84; b1[206]=0.484938; a2[206]=22.7169; b2[206]=2.96118; a3[206]=13.5807; b3[206]=11.5331; a4[206]=5.66016; b4[206]=24.3992; c[206]=13.1991;
element[207]="Pu4+"; a1[207]=35.6493; b1[207]=0.481422; a2[207]=22.646; b2[207]=2.8902; a3[207]=13.3595; b3[207]=11.316; a4[207]=5.18831; b4[207]=21.8301; c[207]=13.1555;
element[208]="Pu6+"; a1[208]=35.1736; b1[208]=0.473204; a2[208]=22.7181; b2[208]=2.73848; a3[208]=14.7635; b3[208]=11.553; a4[208]=2.28678; b4[208]=20.9303; c[208]=13.0582;
element[209]="Am"; a1[209]=36.6706; b1[209]=0.483629; a2[209]=24.0992; b2[209]=3.20647; a3[209]=17.3415; b3[209]=14.3136; a4[209]=3.49331; b4[209]=102.273; c[209]=13.3592;
element[210]="Cm"; a1[210]=36.6488; b1[210]=0.465154; a2[210]=24.4096; b2[210]=3.08997; a3[210]=17.399; b3[210]=13.4346; a4[210]=4.21665; b4[210]=88.4834; c[210]=13.2887;
element[211]="Bk"; a1[211]=36.7881; b1[211]=0.451018; a2[211]=24.7736; b2[211]=3.04619; a3[211]=17.8919; b3[211]=12.8946; a4[211]=4.23284; b4[211]=86.003; c[211]=13.2754;
element[212]="Cf"; a1[212]=36.9185; b1[212]=0.437533; a2[212]=25.1995; b2[212]=3.00775; a3[212]=18.3317; b3[212]=12.4044; a4[212]=4.24391; b4[212]=83.7881; c[212]=13.2674;





for (int k=0; k<NumberAtom; k++)

{
int i=1;
while ((sAtomED->AtomName[k]!=element[i])&(i<AFconst)) {
    i=i+1;
    if (i==AFconst) LWARNING("Do not find atom factor constant for: " + sAtomED->AtomName[k]);
}
    sAtomED->a1[k]=a1[i];
    sAtomED->a2[k]=a2[i];
    sAtomED->a3[k]=a3[i];
    sAtomED->a4[k]=a4[i];
    sAtomED->b1[k]=b1[i];
    sAtomED->b2[k]=b2[i];
    sAtomED->b3[k]=b3[i];
    sAtomED->b4[k]=b4[i];
    sAtomED->c[k]=c[i];


}

}
//-----------------------------------------
//-----------------------------------------


float PDBtoEDM::CalcElectronDens(struct AtomicED sAtomED, int k, float R)
{

    float ED=sAtomED.a1[k]*exp(-4*PI*PI*pow(R,2)/sAtomED.b1[k])/pow(sAtomED.b1[k]/(4*PI),1.5)
    +sAtomED.a2[k]*exp(-4*PI*PI*pow(R,2)/sAtomED.b2[k])/pow(sAtomED.b2[k]/(4*PI),1.5)
    +sAtomED.a3[k]*exp(-4*PI*PI*pow(R,2)/sAtomED.b3[k])/pow(sAtomED.b3[k]/(4*PI),1.5)
    +sAtomED.a4[k]*exp(-4*PI*PI*pow(R,2)/sAtomED.b4[k])/pow(sAtomED.b4[k]/(4*PI),1.5);

    return ED;

}


//-----------------------------------------
//----------find bounding box--------------
//-----------------------------------------
void PDBtoEDM::FindBoundingGeometry(const OBMol mol)
{
std::cout << "Searching size of bounding geometry..."<< std::endl;
size_x=0;
size_y=0;
size_z=0;
for (int i = 1; i <= mol.NumAtoms(); i++)
    {
        OBAtom* a = mol.GetAtom(i);

        float atomx=a->x();
        float atomy=a->y();
        float atomz=a->z();
        float temp1=sqrt(pow(atomx-cx,2));
        float temp2=sqrt(pow(atomy-cy,2));
        float temp3=sqrt(pow(atomz-cz,2));
        if (size_x<temp1) size_x=temp1;
        if (size_y<temp2) size_y=temp2;
        if (size_z<temp3) size_z=temp3;

    }

NumberVoxels_x=2*(size_x+MaxR)/dr;
NumberVoxels_y=2*(size_y+MaxR)/dr;
NumberVoxels_z=2*(size_z+MaxR)/dr;
big_size=0;
if (big_size<2*(size_x+MaxR)) big_size=2*(size_x+MaxR);
if (big_size<2*(size_y+MaxR)) big_size=2*(size_y+MaxR);
if (big_size<2*(size_z+MaxR)) big_size=2*(size_z+MaxR);
Nh=big_size/resol;
NumberVoxel_Structure=2*Nh;
std::cout << "Size of bounding geometry: "<< 2*(size_x+MaxR)<<";"<<2*(size_y+MaxR)<<";"<<2*(size_z+MaxR)<<std::endl;
std::cout << "Cube bounding: "<<big_size<<std::endl;

}
//-----------------------------------------
//-----------------------------------------


void PDBtoEDM::process() {
    if (calculationmode_.isSelected("scattering"))
    {
    resolution_.setVisible(false);
    atoomr_.setVisible(true);

    }
    else
    {
    resolution_.setVisible(true);
    atoomr_.setVisible(false);

    }
}

void PDBtoEDM::ShowGrid() {
    MoleculeCollection* collection = moleculeURLlist_.getMolecules(true);

    for (size_t i = 0; i < collection->size(); i++) {
        Molecule* InputMoll = collection->at(i);
        const OBMol& mol = InputMoll->getOBMol();

        if (mol.NumAtoms()!=0)
        {
            Volume* volume;

            if (calculationmode_.isSelected("scattering"))
                volume = GenerateEDMGrid_ScatteringFactor(InputMoll);
            if (calculationmode_.isSelected("structure"))
                volume = GenerateEDMGrid_StructureFactor(InputMoll);

            //-----------------------------------------
            //--------Set volume identifier------------
            //-----------------------------------------
            std::string url = InputMoll->getOrigin().getURL();
            volume->setOrigin(VolumeURL(url + "_EDM"));

            DensityMapCoProcessor::getSourceProcessor()->addVolume(volume, true, true);

            LWARNING("Density map calculated!");
        }
    }

}

void PDBtoEDM::updateSelection() {
    MoleculeCoProcessor::updateSelection();
    const MoleculeCollection* collection = getInputMoleculeCollection();
    if (collection == 0) {
        LERROR("Collection is NULL at DensityMapManipulation::updateSelection()");
        return;
    }
    moleculeURLlist_.clear();
    for (size_t i = 0; i < collection->size(); i++)
        moleculeURLlist_.addMolecule(collection->at(i));
}


} // namespace

#include "formfinder.h"


std::string FormFinder::loggerCat_ = "homology.FormFinder";

FormFinder::FormFinder()
    : findDomains_("findDomains", "Find Domains")
    , volinport_(Port::INPORT,   "volume", "Electon density map")
    , alignmentList_("alignmentList", "Alignment List")
    , maxDomainsToLoad_("maxDomainsToLoad", "Max domains to load", 3, 1, 5)
    , accuracy_("accuracy", "Bottom value cutoff", 0, 0, 10)
    , weightFactor_("weightFactor", "Weight Factor", 12, 1, 100, Processor::INVALID_PROGRAM)
{
    addProperty(findDomains_);
    addProperty(maxDomainsToLoad_);
    addProperty(accuracy_);
    addProperty(weightFactor_);
    addPort(volinport_);
    findDomains_.onChange(CallMemberAction<FormFinder>(this, &FormFinder::findDomains));
}

void FormFinder::findDomains() 
{
	int	mom_total = 239;
	const VolumeBase* data =  volinport_.getData();
	const Volume* vol = data->clone();

	PointCloud  cloud;
	cloud.VolumeFill(vol, accuracy_.get());	
	cloud.weightfactor  = weightFactor_.get();
	cloud.GetMoments8();
	double *moments = cloud.moments;

//   READ THE DATABASE FILE

	int N;
	FILE * db;

	db = fopen(DB_FILENAME, "r");		// open database file
	if (db==NULL)
	{
		LINFO("failed to open database file!");
		return;
	}
	int err = fscanf(db, "%d", &N);			// read total number of entries
	if (!err)
	{
		LINFO("Moments database file corrupted!");
		return;
	}
	std :: cout << "Database opened, "<< N << "entries";
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


	name       =    new char[N*NAMELEN+1];
	disp       =    new long double[N+1];
	db_moments =    new double[mom_total+1];
	int    part   = 0;
	double factor = 10/N;	

	for (int i = 0; i < N; ++i)
	{	
		if (part != (int)(10.0*i/N))
		{
			std :: cout << part*10 << "\% complete" ;
			part = (int)(10.0*i/N);
		}
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
 	std :: cout << "Got distances, sorting...";
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

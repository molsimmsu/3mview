#include "formfinder.h"


std::string FormFinder::loggerCat_ = "homology.FormFinder";

FormFinder::FormFinder()
    : findDomains_("findDomains", "Find Domains")
    , alignmentList_("alignmentList", "Alignment List")
    , momentsOrder_("momorder", "Order of moments in database", 6, 1, 50)
    , maxDomainsToLoad_("maxDomainsToLoad", "Max domains to load", 3, 1, 5)
    , accuracy_("accuracy", "Bottom value cutoff", 0, 0, 10)
    , weightFactor_("weightFactor", "Weight factor", 12, 1, 100, Processor::INVALID_PROGRAM)
    , volumeURLList_("volumeURLList", "Volume URL List", std::vector<std::string>())
{
    addProperty(volumeURLList_);
    addProperty(findDomains_);
    addProperty(momentsOrder_);
    addProperty(maxDomainsToLoad_);
    addProperty(accuracy_);
    addProperty(weightFactor_);
    findDomains_.onChange(CallMemberAction<FormFinder>(this, &FormFinder::findDomains));
}

void FormFinder::updateSelection() {
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

void FormFinder::findDomains() 
{
    const VolumeCollection* collection = volumeURLList_.getVolumes(true);
    if (collection == 0 || collection->size() == 0) return;
	const VolumeBase* data =  collection->at(0);
	const Volume* vol = data->clone();

	PointCloud  cloud;
	int	mom_total = cloud.GetMomentsNumber(momentsOrder_.get());
	
	std::cout << "Total number of moments: " << mom_total << std::endl;
	
	moments = new double[mom_total];
	cloud.VolumeFill(vol, accuracy_.get());	
	cloud.weightfactor  = weightFactor_.get();
	std :: cout << "Calculating " << momentsOrder_.get() << " order moments (" << mom_total << " pieces) \n";
	cloud.GetMoments(momentsOrder_.get());
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
	std :: cout << "Database opened, "<< N << " entries\n";
	char           tmp;
	char			buff[25];
	char			*name;
	long double	*disp;
	double		*db_moments;
	double 		dt;
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
			std :: cout << part*10 << "\% complete\n" << std :: flush ;
			part = (int)(10.0*i/N);
		}
		fscanf(db, "%s", buff);
		if (strlen(buff)>NAMELEN)	
		{
			printf("Wrong number of moments in database.\n");
			return;			
		}
		for (int cntr = 0; cntr<12; ++cntr)
		{	
			fscanf(db, "%lf", &dt);
		}
	
		disp[i] = 0;
		for (int j=0; j < mom_total; ++j)
		{
			fscanf(db, "%lf", &db_moments[j]);
			disp[i] += (moments[j] - db_moments[j])*(moments[j] - db_moments[j]);
			
		}
		strncpy(&name[NAMELEN*i], buff, NAMELEN);
		disp[i] /= mom_total;
		disp[i] = sqrt(disp[i]);
	}	
	fclose(db); 
 	std :: cout << "Got distances, sorting...\n";
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


    MoleculeCollectionSource* molCollection = MoleculeCoProcessor::getSourceProcessor();
    if (molCollection == 0) return;
    
    std::string path(DOMAIN_LOAD_PATH);
    std::string pdbPath;

    LINFO("Loading domains:\n");
    for (size_t i = 0; i < maxDomainsToLoad_.get(); i++) {
        pdbPath = path;
	   pdbPath += &name[i*NAMELEN];
	   pdbPath += ".pdb";
        LINFO(pdbPath.c_str());
        molCollection->load(pdbPath, true);
    }
    delete[] name;
    delete[] disp;
    delete[] db_moments;
}

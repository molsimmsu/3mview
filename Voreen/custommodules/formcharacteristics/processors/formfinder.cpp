#include "formfinder.h"


std::string FormFinder::loggerCat_ = "homology.FormFinder";

FormFinder::FormFinder()
    : findDomains1_("findDomains1", "Find Domains for Vol")
    , findDomains2_("findDomains2", "Find Domains for Mol")
    , alignmentList_("alignmentList", "Alignment List")
    , maxDomainsToLoad_("maxDomainsToLoad", "Max domains to load", 3, 1, 5)
    , accuracy_("accuracy", "Bottom value cutoff", 0, 0, 10)
    , weightFactor_("weightFactor", "Weight factor", 1, 0.05, 20, Processor::INVALID_PROGRAM)
    , massImportance_("massImportance", "massImportance", 1, 0, 1, Processor::INVALID_PROGRAM)
    , volumeURLList_("volumeURLList", "Volume URL List", std::vector<std::string>())
    , moleculeURLlist_("moleculeURLlist_", "Molecule URL List", std::vector<std::string>())
{
    addProperty(volumeURLList_);
    addProperty(moleculeURLlist_);
    addProperty(findDomains1_);
    addProperty(findDomains2_);
    addProperty(maxDomainsToLoad_);
    addProperty(accuracy_);
    addProperty(weightFactor_);
    addProperty(massImportance_);
    findDomains1_.onChange(CallMemberAction<FormFinder>(this, &FormFinder::findDomainsVol));
    findDomains2_.onChange(CallMemberAction<FormFinder>(this, &FormFinder::findDomainsMol));
}

void FormFinder::updateSelection() {
    DensityMapCoProcessor::updateSelection();
    const VolumeCollection* vcollection = getInputVolumeCollection();
    if (vcollection == 0) {
        LERROR("Collection is NULL at DensityMapManipulation::updateSelection()");
        return;
    }
    volumeURLList_.clear();
    for (size_t i = 0; i < vcollection->size(); i++)
        volumeURLList_.addVolume(vcollection->at(i));

    MoleculeCoProcessor::updateSelection();
    const MoleculeCollection* mcollection = getInputMoleculeCollection();
    if (mcollection == 0) {
        LERROR("Collection is NULL at DensityMapManipulation::updateSelection()");
        return;
    }
    moleculeURLlist_.clear();
    for (size_t i = 0; i < mcollection->size(); i++)
        moleculeURLlist_.addMolecule(mcollection->at(i));

}

void FormFinder::findDomainsVol() 
{
     const VolumeCollection* collection = volumeURLList_.getVolumes(true);
     if (collection == 0 || collection->size() == 0) return;
	const VolumeBase* data =  collection->at(0);
	const Volume* vol = data->clone();

     clock_t        now = clock();
	double weight_factor = weightFactor_.get();

	PointCloud  cloud;
	int	mom_total = cloud.GetMomentsNumber(DB_ORDER);
	
	std::cout << "Total number of moments: " << mom_total << std::endl;

	moments = new double[mom_total];
	cloud.VolumeFill(vol, accuracy_.get());	
	std :: cout << "Calculating " << DB_ORDER << " order moments (" << mom_total << " pieces) \n";
	cloud.GetMoments(DB_ORDER);
	double *moments = cloud.moments;

//   READ THE DATABASE FILE
	int N;
	fstream db(DB_FILENAME, std::ios::in);
	if (db==NULL)
	{
		LINFO("failed to open database file!");
		return;
	}

	db >>  N;			// read total number of entries

	std :: cout << "Database opened, "<< N << " entries\n";
	char			buff[50];
	char			*name;
	long double	*disp;
	double		*db_moments;
	double         temp; 

	disp       =    new long double[N+1];
	name       =    new char[N*NAMELEN+1];
	db_moments =    new double[mom_total+1];

	double    entryscale;
	double	entrymass;

	int    part   = 0;
	double factor = 10/N;	

	std :: cout << "Reading database..\n";
	for (int i = 0; i < N; ++i)
	{	
		if (part != (int)(10.0*i/N))
		{
			std :: cout << part*10 << "\% complete\n" << std :: flush ;
			part = (int)(10.0*i/N);
		}
		db >> buff;
		if (strlen(buff)>NAMELEN)	
		{
			printf("Wrong number of moments in database.\n");
			return;			
		}
		db >> entrymass;

		for (int cntr = 0; cntr<12; ++cntr)
		{	
			db >> temp;
		}

		disp[i] = 0;

		for (int j=0; j < mom_total; ++j)
		{
			db >> db_moments[j];
			disp[i] += (moments[j] - db_moments[j])*(moments[j] - db_moments[j]);
			
		}
		strncpy(&name[NAMELEN*i], buff, NAMELEN);
		disp[i] += massImportance_.get()*(entrymass - cloud.total_weight/weight_factor)*(entrymass - cloud.total_weight/weight_factor);
		disp[i] = sqrt(disp[i]);
	}	
	db.close(); 
 	std :: cout << "Got distances, sorting...\n";	

    for (size_t i = 0; i < maxDomainsToLoad_.get(); i++) 
		{	
	 	    for (size_t j = N-1; j > 0; j--)
		    {
				if (disp[j] < disp[j-1])
				{
	 	   			temp = disp[j];  
					disp[j] = disp[j-1];
					disp[j-1] = temp;
					memcpy(buff, &name[j*NAMELEN], NAMELEN*sizeof(char));   
					memcpy(&name[j*NAMELEN], &name[(j-1)*NAMELEN], NAMELEN*sizeof(char));
					memcpy(&name[(j-1)*NAMELEN], buff, NAMELEN*sizeof(char));
				}
		    }
		}
    

    MoleculeCollectionSource* molCollection = MoleculeCoProcessor::getSourceProcessor();
    if (molCollection == 0) return;
    
    std::string path(DOMAIN_LOAD_PATH);
    std::string pdbPath;

    LINFO("Loading domains:\n");
    for (size_t i = 0; i < maxDomainsToLoad_.get(); i++) {
 	   sprintf(buff, "loading domain: %s [score: %.4Lf]\n", &name[i*NAMELEN], disp[i]);
	   LINFO(buff);
        pdbPath = path;
	   pdbPath += &name[i*NAMELEN];
	   pdbPath += ".pdb";
        LINFO(pdbPath.c_str());
        molCollection->load(pdbPath, true);
    }
    delete[] name;
    delete[] disp;
    delete[] db_moments;

    sprintf(buff, "Time elapsed: %lf\n", ((double)(clock() - now))/CLOCKS_PER_SEC);
    LINFO(buff);
}

void FormFinder::findDomainsMol() 
{
    const MoleculeCollection* collection = moleculeURLlist_.getMolecules(true);
    if (collection == 0 || collection->size() == 0) return;

    const Molecule* data = collection->at(0);
    if (data == 0)
    {
         LWARNING("Molecule is 0. Stop");
         return;
    }

     clock_t        now = clock();
	double weight_factor = weightFactor_.get();


	PointCloud      cloud;
	const Molecule* mol = data->clone();
	int	           mom_total = cloud.GetMomentsNumber(DB_ORDER);

	std::cout << "Total number of moments: " << mom_total << std::endl;
	
	moments = new double[mom_total];
	cloud.MoleculeFill(mol);	

	std :: cout << "Calculating " << DB_ORDER << " order moments (" << mom_total << " pieces) \n";

	cloud.GetMoments(DB_ORDER);
	double *moments = cloud.moments;

//   READ THE DATABASE FILE

	int N;
	fstream db(DB_FILENAME, std::ios::in);
	if (db==NULL)
	{
		LINFO("failed to open database file!");
		return;
	}

	db >>  N;			// read total number of entries

	std :: cout << "Database opened, "<< N << " entries\n";
	char			buff[50];
	char			*name;
	long double	*disp;
	double		*db_moments;
	double         temp; 

	disp       =    new long double[N+1];
	name       =    new char[N*NAMELEN+1];
	db_moments =    new double[mom_total+1];

	double    entryscale;
	double	entrymass;

	int    part   = 0;
	double factor = 10/N;	

	std :: cout << "Reading database..\n";
	for (int i = 0; i < N; ++i)
	{	
		if (part != (int)(10.0*i/N))
		{
			std :: cout << part*10 << "\% complete\n" << std :: flush ;
			part = (int)(10.0*i/N);
		}
		db >> buff;
		if (strlen(buff)>NAMELEN)	
		{
			printf("Wrong number of moments in database.\n");
			return;			
		}
		db >> entrymass;

		for (int cntr = 0; cntr<12; ++cntr)
		{	
			db >> temp;
		}

		disp[i] = 0;

		for (int j=0; j < mom_total; ++j)
		{
			db >> db_moments[j];
			disp[i] += (moments[j] - db_moments[j])*(moments[j] - db_moments[j]);
			
		}
		strncpy(&name[NAMELEN*i], buff, NAMELEN);
		disp[i] += massImportance_.get()*(entrymass - cloud.total_weight/weight_factor)*(entrymass - cloud.total_weight/weight_factor);
		disp[i] = sqrt(disp[i]);
	}	
	db.close(); 
 	std :: cout << "Got distances, sorting...\n";	

    for (size_t i = 0; i < maxDomainsToLoad_.get(); i++) 
		{	
	 	    for (size_t j = N-1; j > 0; j--)
		    {
				if (disp[j] < disp[j-1])
				{
	 	   			temp = disp[j];  
					disp[j] = disp[j-1];
					disp[j-1] = temp;
					memcpy(buff, &name[j*NAMELEN], NAMELEN*sizeof(char));   
					memcpy(&name[j*NAMELEN], &name[(j-1)*NAMELEN], NAMELEN*sizeof(char));
					memcpy(&name[(j-1)*NAMELEN], buff, NAMELEN*sizeof(char));
				}
		    }
		}
    
		
    MoleculeCollectionSource* molCollection = MoleculeCoProcessor::getSourceProcessor();
    if (molCollection == 0) return;
    
    std::string path(DOMAIN_LOAD_PATH);
    std::string pdbPath;

    for (size_t i = 0; i < maxDomainsToLoad_.get(); i++) {
	   sprintf(buff, "loading domain: %s [score: %.4Lf]\n", &name[i*NAMELEN], disp[i]);
	   LINFO(buff);
        pdbPath = path;
	   pdbPath += &name[i*NAMELEN];
	   pdbPath += ".pdb";
        LINFO(pdbPath.c_str());
        molCollection->load(pdbPath, true);
    }
    delete[] name;
    delete[] disp;
    delete[] db_moments;

    sprintf(buff, "Time elapsed: %lf\n", ((double)(clock() - now))/CLOCKS_PER_SEC);
    LINFO(buff);
}


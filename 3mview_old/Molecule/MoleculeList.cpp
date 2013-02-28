#include "MoleculeList.h"

MoleculeList::MoleculeList(QObject *parent) :
    QObject(parent)
{
}

bool MoleculeList::loadFromFile(string path)
{
    return loadOpenStructure(path.c_str());
    return loadOpenBabel(path.c_str());
}

bool MoleculeList::addNewMolecule(EntityHandle h, const char* name, Color c)
{
	if (Scene::Instance().HasNode(name)) return false;

	Entity* e = new Entity(name, h);
	e->SetRenderMode(RenderMode::HSC);
	e->SetColor(c);
	GfxNodeP p(e);
	Scene::Instance().Add(p);

    Molecule* m = new Molecule(e);
    ObjectDispatcher::setName(m);

	emit(addMolecule(m));

	return true;
}

bool MoleculeList::loadOpenStructure(const char* path)
{
	EntityHandle h = ost::io::LoadEntity(path);
	addNewMolecule(h, path, Color(0.2, 1, 0.2));

	return true;
}

bool MoleculeList::loadOpenBabel(const char* path)
{
	// Чтение
	OBConversion obconversion;
	obconversion.SetInFormat("PDB");
	OBMol mol;
	qDebug() << "Reading file" << path;
	if (!obconversion.ReadFile(&mol, path))
	{
		qDebug() << "Error reading" << path;
		return 1;
	}
	qDebug() << "Reading file done";

	StrideReader stride(path);

	// Молекула OpenStructure
	EntityHandle eh1 = CreateEntity();

	// Редактор молекулы OpenStructure
	XCSEditor eh1Edit = eh1.EditXCS();

	// Списки цепей, аминокислот, атомов OpenStructure. Порядок расположения
	// элементов соответствует исходной молекуле OpenBabel
	QVector<ChainHandle> ceh;
	QVector<ResidueHandle> reh;
	QVector<AtomHandle> aeh;

	// Считываем аминокислоты. Создаем цепи аминокислот.
	qDebug() << "Reading residues" << mol.NumResidues();
	for (unsigned int i = 0; i < mol.NumResidues(); i++)
	{
		qDebug() << "start" << i;
		OBResidue* r = mol.GetResidue(i);
		qDebug() << "GetResidue" << i;

		unsigned int chainNum = r->GetChainNum(); // Номер цепи аминокислоты
		std::string residueName = r->GetName(); // Имя аминокислоты (GLY, HIS ...)
		qDebug() << "GetName" << i;
		// Если номер цепи превышает количество цепей, создаем новую цепь
		if (chainNum > ceh.size())
		{
			std::string chainName(1, r->GetChain()); // Имя цепи (A, B, C ...)
			ceh.push_back(eh1Edit.InsertChain(chainName));
		}
		qDebug() << "InsertChain" << i;
		// Добавляем аминокислоту в цепь
		ChainHandle& chain = ceh[chainNum - 1];
		qDebug() << "ChainHandle" << i << chain.IsValid();
		reh.push_back(eh1Edit.AppendResidue(chain, residueName.c_str()));
		qDebug() << "AppendResidue" << i;
	}
	qDebug() << "Reading residues done";

	// Считываем атомы. Добавляем атомы в аминокислоты.
	qDebug() << "Reading atoms" << mol.NumAtoms();
	for (unsigned int i = 0; i < mol.NumAtoms(); i++)
	{
		OBAtom* a = mol.GetAtom(i);

		// В случае пустого атома добавляем "заглушку" в список для сохранения
		// нумерации. Это важно на следующем шаге при создании связей
		if (!a)
		{
			AtomHandle dummy;
			aeh.push_back(dummy);
			continue;
		}

		Vec3 pos(a->x(), a->y(), a->z()); // Координаты атома
		ResidueHandle res = reh[a->GetResidue()->GetIdx()]; // Аминокислота

		// Тип атома в формате OpenBabel
		std::string type = a->GetType();

		ttab.SetFromType("INT"); // Внутренний формат OpenBabel

		// Название хим. элемента
		std::string el;
		ttab.SetToType("XYZ");
		ttab.Translate(el, type);

		// Имя атома в формате PDB (CA, CB, CG1 ...)
		std::string name = a->GetResidue()->GetAtomID(a);
		name.erase(remove(name.begin(), name.end(), ' '), name.end());

		AtomHandle ah = eh1Edit.InsertAtom(res, name.c_str(), pos, el.c_str());
		ah.SetFloatProp("trace_rad", 1);

		if (name.compare("CA") == 0)
		{
			res.SetCentralAtom(ah);
			res.SetChemClass(ChemClass(ChemClass::PEPTIDE_LINKING));
		}

		aeh.push_back(ah);
	}
	qDebug() << "Reading atoms done";

	// Считываем связи между атомами
	qDebug() << "Reading bonds" << mol.NumBonds();
	OBBondIterator bond = mol.BeginBonds();
	OBBondIterator bondEnd = mol.EndBonds();
	for (unsigned int i = 0; bond != bondEnd; ++bond, ++i)
	{
		OBBond* b = *bond;
		if (!b) continue;
		AtomHandle a1 = aeh[b->GetBeginAtom()->GetIdx()];
		AtomHandle a2 = aeh[b->GetEndAtom()->GetIdx()];

		if (a1.IsValid() && a2.IsValid())
		{
			// Very slow!
			eh1Edit.Connect(a1, a2);
		}
		if (i % 100 == 0) qDebug() << "Handling bond" << i;
	}
	qDebug() << "Reading bonds done";

	for (unsigned int chainNum = 0; chainNum < stride.numChains(); chainNum++)
	for (unsigned int residueNum = 0; residueNum < stride.numResidues(chainNum); residueNum++)
	{
		ChainHandle chain = eh1.GetChainList().at(chainNum);
		ResidueHandle residue = chain.GetResidueByIndex(residueNum);
		char code = stride.secondaryStructure(chainNum, residueNum);

		ost::mol::SecStructure ss;

		switch(code)
        {
		case 'H': ss = SecStructure(SecStructure::ALPHA_HELIX);
			break;
		case 'B': ss = SecStructure(SecStructure::BETA_BRIDGE);
			break;
		case 'G': ss = SecStructure(SecStructure::THREE_TEN_HELIX);
			break;
		case 'T': ss = SecStructure(SecStructure::TURN);
			break;
		case 'C': ss = SecStructure(SecStructure::COIL);
			break;
		case 'E': ss = SecStructure(SecStructure::BETA_BRIDGE);
			break;
		}

        residue.SetSecStructure(ss);
	}

	addNewMolecule(eh1, path, Color(0.2, 1, 0.2));

	return true;
}

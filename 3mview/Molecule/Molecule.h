#ifndef MOLECULE_H
#define MOLECULE_H

#include "DataObject.h"
#include "OpenStructure.h"
#include "ObjectDispatcher.h"

class Molecule : public DataObject
{
public:
    Molecule(ost::gfx::Entity* e, QObject *parent = 0);

    virtual string typeName() { return string("Molecule"); }

	void setVisible(bool);
	void setSelection(bool state);

private:
	ost::gfx::Entity* entity;
};

#endif // MOLECULE_H

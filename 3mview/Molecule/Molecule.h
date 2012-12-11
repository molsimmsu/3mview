#ifndef MOLECULE_H
#define MOLECULE_H

#include "DataObject.h"
#include "ObjectDispatcher.h"

#include "ost/mol/entity_handle.hh"
#include "ost/gfx/entity.hh"

class Molecule : public DataObject
{
public:
    Molecule(ost::gfx::Entity* e);

    virtual string typeName() { return string("Molecule"); }

	void setVisible(bool);
	void setSelection(bool state);

private:
	ost::gfx::Entity* entity;
};

#endif // MOLECULE_H

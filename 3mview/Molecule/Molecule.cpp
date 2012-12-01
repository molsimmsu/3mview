#include "Molecule.h"

Molecule::Molecule(const char* name, ost::gfx::Entity* e, QObject *parent) :
	_name(name),
	entity(e),
	QObject(parent)
{
}

void Molecule::setVisible(bool state)
{
	entity->SetVisible("", state);
}

void Molecule::setSelection(bool state)
{
	ost::mol::EntityHandle h = entity->GetEntity();
	ost::mol::EntityView v;

	if (state) v = h.Select("");
	else v = h.Select("rnum < 0");

	entity->SetSelection(v);
}

const char* Molecule::name()
{
	return _name;
}

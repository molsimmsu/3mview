#include "Molecule.h"

Molecule::Molecule(ost::gfx::Entity* e) :
    entity(e)
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

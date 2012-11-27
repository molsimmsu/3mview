#ifndef MOLECULE_H
#define MOLECULE_H

#include <QObject>
#include "OpenStructure.h"

class Molecule : public QObject
{
Q_OBJECT
public:
	explicit Molecule(const char* name, ost::gfx::Entity* e, QObject *parent = 0);
	const char* name();
	void setVisible(bool);
	void setSelection(bool state);
signals:

public slots:

private:
	const char* _name;
	ost::gfx::Entity* entity;
};

#endif // MOLECULE_H

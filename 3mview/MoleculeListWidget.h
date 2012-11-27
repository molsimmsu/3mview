#ifndef MOLECULELISTWIDGET_H
#define MOLECULELISTWIDGET_H

#include <QtGui>
#include <QVector>
#include "MoleculeList.h"
#include "Molecule.h"

class MoleculeListWidget : public QListWidget
{
Q_OBJECT
public:
	explicit MoleculeListWidget(MoleculeList *ml, QListWidget *parent = 0);

signals:

public slots:
	void addMolecule(Molecule* m);
	void itemSelected(QListWidgetItem* i);
	void itemChanged(QListWidgetItem* i);

private:
	MoleculeList* ml;
	QVector<Molecule*> mols;
	QVector<QListWidgetItem*> items;

	Molecule* getMolecule(QListWidgetItem* i);
	void deselectAll();
};

#endif // MOLECULELISTWIDGET_H

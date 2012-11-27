#include "MoleculeListWidget.h"

MoleculeListWidget::MoleculeListWidget(MoleculeList *ml, QListWidget *parent) :
	ml(ml),
	QListWidget(parent)
{
	this->setWindowTitle("Molecules");
	resize(400, 200);

	connect(ml, SIGNAL(addMolecule(Molecule*)), SLOT(addMolecule(Molecule*)));
	connect(this, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(itemSelected(QListWidgetItem*)));
	connect(this, SIGNAL(itemChanged(QListWidgetItem*)), SLOT(itemChanged(QListWidgetItem*)));
}

void MoleculeListWidget::addMolecule(Molecule* m)
{
	QListWidgetItem* i = new QListWidgetItem(m->name(), this);
	i->setCheckState(Qt::Checked);
	items.push_back(i);
	mols.push_back(m);
}

void MoleculeListWidget::itemSelected(QListWidgetItem* i)
{
	Molecule* m = getMolecule(i);
	if (!m) return;

	deselectAll();
	m->setSelection(true);
	qDebug() << "Item selected";
}

void MoleculeListWidget::deselectAll()
{
	for (unsigned int i = 0; i < mols.size(); i++)
		mols[i]->setSelection(false);
}

void MoleculeListWidget::itemChanged(QListWidgetItem* i)
{
	Molecule* m = getMolecule(i);
	if (!m) return;

	if (i->checkState() == Qt::Checked)	m->setVisible(true);
	else m->setVisible(false);

}

Molecule* MoleculeListWidget::getMolecule(QListWidgetItem* i)
{
	unsigned int index = std::find(items.begin(), items.end(), i) - items.begin();

	if (index >= mols.size()) return NULL;
	return mols[index];
}

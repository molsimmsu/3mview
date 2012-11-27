#include "DensityMapListWidget.h"

DensityMapListWidget::DensityMapListWidget(DensityMapList *dl, QListWidget *parent) :
	dl(dl),
	QListWidget(parent)
{
	this->setWindowTitle("Density Maps");
	resize(400, 200);

	connect(dl, SIGNAL(addDensityMap(DensityMap*)), SLOT(addDensityMap(DensityMap*)));
	connect(this, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(itemSelected(QListWidgetItem*)));
	connect(this, SIGNAL(itemChanged(QListWidgetItem*)), SLOT(itemChanged(QListWidgetItem*)));
}

void DensityMapListWidget::addDensityMap(DensityMap* m)
{
	QListWidgetItem* i = new QListWidgetItem(m->name(), this);
	i->setCheckState(Qt::Checked);
	items.push_back(i);
	mols.push_back(m);
}

void DensityMapListWidget::keyPressEvent(QKeyEvent* e)
{
	if (e->key() == Qt::Key_Delete) {
		QListWidgetItem* i = this->selectedItems()[0];
		dl->remove(getDensityMap(i));
	}
}

void DensityMapListWidget::itemSelected(QListWidgetItem* i)
{
	DensityMap* m = getDensityMap(i);
	if (!m) return;

	deselectAll();
	m->setSelection(true);
	qDebug() << "Item selected";

	DensityMapWidget* w = new DensityMapWidget(m);
	w->show();
}

void DensityMapListWidget::deselectAll()
{
	for (unsigned int i = 0; i < mols.size(); i++)
		mols[i]->setSelection(false);
}

void DensityMapListWidget::itemChanged(QListWidgetItem* i)
{
	DensityMap* m = getDensityMap(i);
	if (!m) return;

	if (i->checkState() == Qt::Checked)	m->setVisible(true);
	else m->setVisible(false);

}

DensityMap* DensityMapListWidget::getDensityMap(QListWidgetItem* i)
{
	unsigned int index = std::find(items.begin(), items.end(), i) - items.begin();

	if (index >= mols.size()) return NULL;
	return mols[index];
}

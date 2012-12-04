#include "DensityMapListWidget.h"

DensityMapListWidget::DensityMapListWidget(DensityMapList *dl, QListWidget *parent) :
	dl(dl),
	QListWidget(parent)
{
	this->setWindowTitle("Density Maps");
    setGeometry(750, 135, 400, 200);

	connect(dl, SIGNAL(addDensityMap(DensityMap*)), SLOT(addDensityMap(DensityMap*)));
	connect(this, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(itemSelected(QListWidgetItem*)));
	connect(this, SIGNAL(itemChanged(QListWidgetItem*)), SLOT(itemChanged(QListWidgetItem*)));
}

void DensityMapListWidget::addDensityMap(DensityMap* m)
{
    QListWidgetItem* i = new QListWidgetItem(m->name().c_str(), this);
	i->setCheckState(Qt::Checked);
	items.push_back(i);
	mols.push_back(m);
}

// Выполнение действия над одной выбранной картой
void DensityMapListWidget::keyPressEvent(QKeyEvent* e)
{
    QListWidgetItem* i = this->selectedItems()[0];
    qDebug() << "Selected item text is" << i->text();
    DensityMap* map = getDensityMap(i);
    qDebug() << "Selected map" << map->name().c_str();

    // Удаление карты
    if (e->key() == Qt::Key_Delete)
    {
        dl->remove(map);
	}
    // Сегментация карты
    else if (e->key() == Qt::Key_S)
    {
        qDebug() << "Segment map";
        SegmentAlgorithm* sa = new ThresholdDivision(2);
        vector <DensityMap*> maps = sa->apply(map);
        for (unsigned int i = 0; i < maps.size(); i++)
        {
            DensityMap* m = maps[i];
            ObjectDispatcher::setName(m);
            ObjectDispatcher::setColor(m);
            dl->add(m);
            m->addToScene();
        }
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

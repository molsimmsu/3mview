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
    QListWidgetItem* selectedItem;
    DensityMap* map;

    if (this->selectedItems().size() > 0)
    {
        selectedItem = this->selectedItems()[0];
        qDebug() << "Selected item text is" << selectedItem->text();
        map = getDensityMap(selectedItem);
        qDebug() << "Selected map" << map->name().c_str();
    }

    // Удаление карты
    if (e->key() == Qt::Key_Delete && map)
    {
        dl->remove(map);
        delete selectedItem;
	}
    // Сегментация карты
    else if (e->key() == Qt::Key_S && map)
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
        map->setVisible(false);
    }
    // Создание случайной тестовой карты
    else if (e->key() == Qt::Key_R)
    {
        DensityMap* m = new DensityMap(5, 50, 50);
        ObjectDispatcher::setColor(m);
        ObjectDispatcher::setName(m);
        m->randomize();
        m->addToScene();
        dl->add(m);
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
    if (i == NULL) return NULL;

	unsigned int index = std::find(items.begin(), items.end(), i) - items.begin();

	if (index >= mols.size()) return NULL;
	return mols[index];
}

#include "DensityMapSegmentWidget.h"

DensityMapSegmentWidget::DensityMapSegmentWidget(DensityMapSegment *dms, QWidget *parent) :
    QWidget(parent)
{
    this->dms = dms;
    connect(dms, SIGNAL(update()), SLOT(update()));
    setGeometry(50, 100, 300, 300);
    setWindowTitle(trUtf8("Модуль сегментации карт ЭП"));

    mapList = new QListWidget(this);
    mapList->setGeometry(5, 5, 290, 100);

    algList = new QComboBox(this);
    algList->setGeometry(5, 115, 290, 30);
    algList->addItem("Threshold division");
}

void DensityMapSegmentWidget::update()
{
    mapList->clear();
    for (unsigned int i = 0; i < dms->size(); i++)
    {
        DensityMap* map = dms->at(i);
        QListWidgetItem* it = new QListWidgetItem(map->name().c_str(), mapList);
    }
}

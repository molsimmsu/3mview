#include "SegmentDensityMapWidget.h"

SegmentDensityMapWidget::SegmentDensityMapWidget(QWidget *parent) :
    QWidget(parent)
{
    setGeometry(50, 100, 400, 300);
    setWindowTitle(trUtf8("Модуль сегментации карт ЭП"));
}

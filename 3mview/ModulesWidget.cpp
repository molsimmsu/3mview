#include "ModulesWidget.h"

ModulesWidget::ModulesWidget(QWidget *parent) :
    QWidget(parent)
{
    setGeometry(750, 535, 400, 200);
    setWindowTitle(trUtf8("Модули"));

    segmentBtn = new QPushButton(trUtf8("Сегментация карт ЭП"), this);
    segmentBtn->setGeometry(5, 5, 170, 25);
    connect(segmentBtn, SIGNAL(pressed()), SLOT(openModule()));
}

void ModulesWidget::openModule()
{
    if (sender() == segmentBtn)
    {
        SegmentDensityMapWidget* w = new SegmentDensityMapWidget();
        w->show();
    }
}

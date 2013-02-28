#ifndef MODULESWIDGET_H
#define MODULESWIDGET_H

#include <QtGui>
#include "DensityMap/DensityMapSegmentWidget.h"
#include "DensityMap/DensityMapSegment.h"

class ModulesWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ModulesWidget(QWidget *parent = 0);
    
signals:
    
public slots:
    void openModule();

private:
    QPushButton* segmentBtn;
    
};

#endif // MODULESWIDGET_H

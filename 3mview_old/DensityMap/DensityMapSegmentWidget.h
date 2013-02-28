#ifndef SEGMENTDENSITYMAPWIDGET_H
#define SEGMENTDENSITYMAPWIDGET_H

#include <QtGui>
#include "DensityMapSegment.h"

class DensityMapSegmentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DensityMapSegmentWidget(DensityMapSegment* dms, QWidget *parent = 0);
    
signals:
    
public slots:
    void update();

private:
    QListWidget* mapList;
    QComboBox* algList;
    DensityMapSegment* dms;
};

#endif // SEGMENTDENSITYMAPWIDGET_H

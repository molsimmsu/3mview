#ifndef SEGMENTDENSITYMAPWIDGET_H
#define SEGMENTDENSITYMAPWIDGET_H

#include <QtGui>

class SegmentDensityMapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SegmentDensityMapWidget(QWidget *parent = 0);
    
signals:
    
public slots:

private:
    QListWidget* listWidget;
    
};

#endif // SEGMENTDENSITYMAPWIDGET_H

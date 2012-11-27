#ifndef DENSITYMAPWIDGET_H
#define DENSITYMAPWIDGET_H

#include <QtGui>
#include "DensityMap.h"

class DensityMapWidget : public QWidget
{
Q_OBJECT
public:
	explicit DensityMapWidget(DensityMap* map, QWidget *parent = 0);

signals:

public slots:
	void setLevel(int);
	void changeColor(QColor);

private:
	DensityMap* map;
};

#endif // DENSITYMAPWIDGET_H

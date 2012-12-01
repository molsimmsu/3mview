#ifndef DENSITYMAPLISTWIDGET_H
#define DENSITYMAPLISTWIDGET_H

#include <QtGui>
#include <QVector>
#include "DensityMapList.h"
#include "DensityMap.h"
#include "DensityMapWidget.h"

class DensityMapListWidget : public QListWidget
{
Q_OBJECT
public:
	explicit DensityMapListWidget(DensityMapList *dl, QListWidget *parent = 0);

signals:

public slots:
	void addDensityMap(DensityMap* m);
	void itemSelected(QListWidgetItem* i);
	void itemChanged(QListWidgetItem* i);


private:
	DensityMapList* dl;
	QVector<DensityMap*> mols;
	QVector<QListWidgetItem*> items;

	DensityMap* getDensityMap(QListWidgetItem* i);
	void deselectAll();
	virtual void keyPressEvent(QKeyEvent* e);
};

#endif // DENSITYMAPLISTWIDGET_H

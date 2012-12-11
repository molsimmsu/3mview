#include "DensityMapWidget.h"

DensityMapWidget::DensityMapWidget(DensityMap* map, QWidget *parent) :
	map(map),
    QWidget(parent)
{
	setWindowTitle("Density map settings");
	QSlider* s = new QSlider(Qt::Horizontal, this);

	connect(s, SIGNAL(sliderMoved(int)), SLOT(setLevel(int)));

	QColorDialog* c = new QColorDialog(this);
	QPushButton* b = new QPushButton("color", this);
	b->move(5, 30);

	connect(b, SIGNAL(clicked()), c, SLOT(show()));
	connect(c, SIGNAL(currentColorChanged(QColor)), SLOT(changeColor(QColor)));
}

void DensityMapWidget::setLevel(int l)
{
	double min = map->minLevel();
	double max = map->maxLevel();
	map->setLevel(min + (max - min) * l / 100.);
}


void DensityMapWidget::changeColor(QColor c)
{
    qDebug() << "color" << c.redF() << c.greenF() << c.blueF();
	map->setColor(c.redF(), c.greenF(), c.blueF());
}

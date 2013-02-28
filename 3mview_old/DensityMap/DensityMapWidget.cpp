#include "DensityMapWidget.h"

DensityMapWidget::DensityMapWidget(DensityMap* map, QWidget *parent) :
	map(map),
    QWidget(parent)
{
    setWindowTitle(trUtf8("Настройки карты ЭП"));

    levelSlider = new QSlider(Qt::Horizontal, this);
    colorDialog = new QColorDialog(this);
    colorButton = new QPushButton(trUtf8("Цвет"), this);

    this->setFixedSize(150, 90);
    levelSlider->setGeometry(10, 5, 130, 25);
    colorButton->setGeometry(50, 60, 50, 25);

    connect(levelSlider, SIGNAL(valueChanged(int)), SLOT(setLevel(int)));
    connect(colorButton, SIGNAL(clicked()), colorDialog, SLOT(show()));
    connect(colorDialog, SIGNAL(currentColorChanged(QColor)), SLOT(changeColor(QColor)));

    double min = map->minLevel();
    double max = map->maxLevel();
    levelMin = new QLabel(QString::number(min, 'f', 1), this);
    levelCur = new QLabel(QString::number(map->getLevel(), 'f', 2), this);
    levelMax = new QLabel(QString::number(max, 'f', 1), this);
    levelMin->move(5, 30);
    levelCur->move(66, 30);
    levelMax->move(128, 30);
    levelSlider->setSliderPosition((map->getLevel() - min) / (max - min) * levelSlider->width());
}

void DensityMapWidget::setLevel(int l)
{
	double min = map->minLevel();
	double max = map->maxLevel();
    map->setLevel(min + (max - min) * l / levelSlider->width());
    levelCur->setText(QString::number(map->getLevel(), 'f', 2));
}


void DensityMapWidget::changeColor(QColor c)
{
	map->setColor(c.redF(), c.greenF(), c.blueF());
}

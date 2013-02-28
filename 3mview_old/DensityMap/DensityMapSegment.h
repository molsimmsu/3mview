#ifndef DENSITYMAPSEGMENT_H
#define DENSITYMAPSEGMENT_H

#include <QObject>
#include "DensityMap.h"
#include <vector>
using namespace std;

class DensityMapSegment : public QObject
{
    Q_OBJECT
public:
    explicit DensityMapSegment(QObject *parent = 0);
    void send(DensityMap* map);
    unsigned int size();
    DensityMap* at(unsigned int i);
    
signals:
    void update();
    
public slots:

private:
    vector<DensityMap*> maps;
    
};

#endif // DENSITYMAPSEGMENT_H

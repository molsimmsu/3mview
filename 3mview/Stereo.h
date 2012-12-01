#ifndef STEREO_H
#define STEREO_H

#include <QObject>
#include "OpenStructure.h"

class Stereo : public QObject
{
    Q_OBJECT
public:
    explicit Stereo(QObject *parent = 0);
    bool activate();
    
signals:
    
public slots:
    
};

#endif // STEREO_H

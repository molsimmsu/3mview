#include "Stereo.h"
#include <QDebug>

Stereo::Stereo(QObject *parent) :
    QObject(parent)
{

}

bool Stereo::activate()
{
    ost::gfx::Scene& scene = ost::gfx::Scene::Instance();

    /// \brief set stereo mode
    /// one of 0 (off), 1 (quad-buffered) 2 (interlaced (for special monitors))
    scene.SetStereoMode(1);
    qDebug() << "Stereo mode is" << scene.GetStereoMode();

    /// \brief invert stereo eyes for stereo mode=0
    scene.SetStereoFlip(false);

    /// \brief set stereo algorithm
    /// one of 0 (default) or 1
    scene.SetStereoAlg(0);

      /// \brief set stereo distance offset from COR (center of rotation)
    scene.SetStereoDistance(30);

    /// \brief set stereo eye distance
    scene.SetStereoIOD(6);

    /// \brief stereo view mode
    /// one of 0 (center), -1 (left), 1 (right)
    scene.SetStereoView(0);
}

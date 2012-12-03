#include "DensityMap.h"

DensityMap::DensityMap(string fileName, DensityMap* parent) :
    DataObject(),
    _mapIso(NULL)
{
    _map = ost::io::LoadImage(fileName.c_str());
}

DensityMap::DensityMap(int xSize, int ySize, int zSize, DensityMap* parent):
    DataObject(),
    _mapIso(NULL)
{
    _map = ost::img::CreateImage(ost::img::Size(xSize, ySize, zSize), ost::img::Point(0, 0, 0));
}

void DensityMap::addToScene()
{
    if (_mapIso != NULL) return; // Карта уже добавлена в сцену

    _mapIso = new ost::gfx::MapIso(name().c_str(), _map, 0.5);
    _mapIso->SetRenderMode(ost::gfx::RenderMode::FILL);
    _mapIso->SetColor(ost::gfx::Color(color().r(), color().g(), color().b()));
    _node = ost::gfx::GfxNodeP(_mapIso);

    ost::gfx::Scene::Instance().Add(_node);
}

void DensityMap::randomize()
{
    //im.ApplyIP(Randomize());
}

void DensityMap::setVisible(bool state)
{
    _mapIso->SetVisible(state);
}

void DensityMap::setLevel(double level)
{
    _mapIso->SetLevel(level);
}

void DensityMap::setSelection(bool state)
{

}

double DensityMap::minLevel()
{
    return _mapIso->GetMinLevel();
}

double DensityMap::maxLevel()
{
    return _mapIso->GetMaxLevel();
}

double DensityMap::setColor(double r, double g, double b)
{
    _mapIso->SetColor(ost::gfx::Color(color().r(), color().g(), color().b()));
}

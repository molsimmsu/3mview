#include "DensityMapList.h"

DensityMapList::DensityMapList(QObject *parent) :
	QObject(parent)
{
}

bool DensityMapList::addNewDensityMap(ImageHandle h, const char* name, Color c)
{
	if (Scene::Instance().HasNode(name)) return false;

	MapIso* map = new MapIso(name, h, 0.5);
	map->SetRenderMode(RenderMode::FILL);
	map->SetColor(c);
	GfxNodeP map1P(map);
	Scene::Instance().Add(map1P);

	DensityMap* m = new DensityMap(name, map);
	maps.push_back(m);

	emit(addDensityMap(m));

	return true;
}

void DensityMapList::remove(DensityMap* m)
{
	Scene::Instance().Remove(m->name());
	std::remove(maps.begin(), maps.end(), m);
}

bool DensityMapList::loadFromFile(const char* path)
{
	std::string str(path);
	ImageHandle im = ost::io::LoadImage(str.c_str());
	addNewDensityMap(im, path, Color(1, 0.5, 0.5));

	return true;
}
#include "Geometry.h"

Geometry::~Geometry()
{
    for (auto& it : Vertices)
    {
        delete it.second;
    }
    
    while (Polygons.size() > 0)
    {
        Polygon* poly = Polygons.back();
        Polygons.pop_back();
        delete poly;
    }
}

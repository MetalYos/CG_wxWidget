#include "Geometry.h"

Geometry::~Geometry()
{
    // Delete BBox verticies and polygons
    while (BoundingBoxPolygons.size() > 0)
    {
        Polygon* poly = BoundingBoxPolygons.back();
        BoundingBoxPolygons.pop_back();

        for (Vertex* it : poly->Vertices)
            delete it;

        delete poly;
    }

    // Delete vertices
    for (auto& it : Vertices)
    {
        delete it.second;
    }
    
    // Delete polygons
    while (Polygons.size() > 0)
    {
        Polygon* poly = Polygons.back();
        Polygons.pop_back();
        delete poly;
    }
}

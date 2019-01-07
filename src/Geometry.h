#pragma once

#include "pch.h"
#include <unordered_map>

// Forward decleration
struct Vertex;

struct Polygon
{
    std::vector<Vertex*> Vertices;
    Vec4 Normal;
    Vec4 Center;
};

struct Vertex
{
    int PositionID;
    int TexCoordID;
    int NormalID;
    std::vector<Polygon*> NeighborPolys;

    Vertex(int posID = -1, int texCoordID = -1, int normalID = -1)
        : PositionID(posID), TexCoordID(texCoordID), NormalID(normalID) {}
};

class Geometry
{
public:
    Geometry() {}
    ~Geometry();

public:
    std::vector<Polygon*> Polygons;
    std::unordered_map<int, Vertex*> Vertices;
};
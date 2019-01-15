#pragma once

#include "pch.h"
#include "Geometry.h"

class Model
{
public:
    Model();
    ~Model();

    void LoadFromFile(const std::string& filename);

    void AddGeometry(Geometry* geo);
    const std::vector<Geometry*>& GetGeometries() const;

    const Mat4& GetObjectToWorldTransform() const;
    const Mat4& GetViewTransform() const;
    void Translate(const Mat4& T, int space = ID_SPACE_OBJECT);
    void Rotate(const Mat4& R, int space = ID_SPACE_OBJECT);
    void Scale(const Mat4& S, int space = ID_SPACE_OBJECT);

private:
    Vec4 CalculatePolyNormal(Polygon* p) const;
    Vec4 CalculateVertexNormal(Vertex* v) const;
    void CalculateVertexNormals(Geometry* geo);
    void AddVertexToPoly(Geometry* geo, Polygon* poly, int posID, int normID, int texID);

public:
    std::vector<Vec4> VertexTexCoords;
    std::vector<Vec4> VertexPositions;
    std::vector<Vec4> VertexNormals;

private:
    std::vector<Geometry*> geos;
    Mat4 objectToWorld;
    Mat4 viewTransform;
};
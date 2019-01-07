#include "Model.h"

#include <fstream>
#include <sstream>
#include <exception>

Model::Model()
{
    VertexPositions.reserve(10);
    VertexNormals.reserve(10);
    VertexTexCoords.reserve(10);
}

Model::~Model()
{
    while (geos.size() > 0)
    {
        Geometry* geo = geos.back();
        geos.pop_back();
        delete geo;
    }
}

void Model::LoadFromFile(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
    {
        LOG_ERROR("{0} was not opened!", filename.c_str());
        // TODO: throw exception
    }

    std::string line;
    Geometry* geo = NULL;
    while (getline(file, line))
    {
        std::stringstream ss(line);
        std::string lineType;
        ss >> std::ws >> lineType;

        if (lineType == "g")
        {
            // Add Geometry to model
            if (geo != NULL)
                AddGeometry(geo);

            // Create Geometry
            geo = new Geometry();
        }
        else if (lineType == "v")
        {
            Vec4 position;
            ss >> position[0] >> std::ws >> position[1] >> std::ws >> position[2];
            position[3] = 1.0;
            
            VertexPositions.push_back(position);
        }
        else if (lineType == "vt")
        {
            Vec4 texCoords;
            ss >> texCoords[0] >> std::ws >> texCoords[1];
            if (ss.peek() == ' ')
                ss >> std::ws >> texCoords[2];

            VertexTexCoords.push_back(texCoords);
        }
        else if (lineType == "vn")
        {
            Vec4 normal;
            ss >> normal[0] >> std::ws >> normal[1] >> std::ws >>  normal[2];

            VertexNormals.push_back(normal);
        }
        else if ((lineType == "f") && (geo != NULL))
        {
            Polygon* poly = new Polygon();

            // Construct vertex
            char c;
            while (!ss.eof())
            {
                int posID, normID, texID;

                ss >> std::ws >> posID >> std::ws;
                --(posID);

                if (ss.peek() != '/')
                {
                    AddVertexToPoly(geo, poly, posID, normID, texID);
                    continue;
                }
                
                ss >> c >> std::ws;
                if (ss.peek() == '/')
                {
                    ss >> c >> std::ws >> normID;
                    --(normID);
                    AddVertexToPoly(geo, poly, posID, normID, texID);
                    continue;
                }
                else
                {
                    ss >> texID;
                    --(texID);
                }
                
                if (ss.peek() != '/')
                {
                    AddVertexToPoly(geo, poly, posID, normID, texID);
                    continue;
                }
                    
                ss >> c >> normID;
                --(normID);

                AddVertexToPoly(geo, poly, posID, normID, texID);
            }

            // Calculate Normal
            poly->Normal = CalculatePolyNormal(poly);

            // Add poly to polygons vector
            geo->Polygons.push_back(poly);
        }
        else
        {
            // Do nothing
        }
    }
    file.close();

    AddGeometry(geo);
}

void Model::AddGeometry(Geometry* geo)
{
    CalculateVertexNormals(geo);
    geos.push_back(geo);
}

const std::vector<Geometry*>& Model::GetGeometries() const
{
    return geos;
}

Mat4 Model::GetTransform() const
{
    return transform;
}

void Model::Translate(const Mat4& T, bool objectSpace)
{
    if (objectSpace)
        transform = T * transform;
    else
        transform = transform * T;
}

void Model::Rotate(const Mat4& R, bool objectSpace)
{
    if (objectSpace)
        transform = R * transform;
    else
        transform = transform * R;
}

void Model::Scale(const Mat4& S, bool objectSpace)
{
    if (objectSpace)
        transform = S * transform;
    else
        transform = transform * S;
}

Vec4 Model::CalculatePolyNormal(Polygon* p) const
{
    Vec4 normal(0.0, 0.0, 1.0, 0.0);
    
    if (p->Vertices.size() >= 3)
    {
        Vec4 u = VertexPositions[p->Vertices[0]->PositionID];
        Vec4 v = VertexPositions[p->Vertices[1]->PositionID];
        Vec4 w = VertexPositions[p->Vertices[2]->PositionID];

        Vec4 e1 = v - u;
        Vec4 e2 = w - v;

        if (p->Vertices.size() == 4)
        {
            Vec4 z = VertexPositions[p->Vertices[3]->PositionID];

            if (Vec4::Length3(e1) < AL_DBL_EPSILON)
            {
                e1 = e2;
                e2 = z - w;
            }
            else if (Vec4::Length3(e2) < AL_DBL_EPSILON)
            {
                e2 = z - w;
            }
        }

        normal = Vec4::Cross(e1, e2); // TODO: maybe change order (or negate)
        normal = Vec4::Normalize3(normal);
        normal[3] = 0.0;
    }
    
    return normal;
}

Vec4 Model::CalculateVertexNormal(Vertex* v) const
{
    if (v->NeighborPolys.size() == 0)
        return Vec4(0.0, 0.0, 1.0, 0.0);
    
    Vec4 normal(0.0, 0.0, 0.0, 0.0);
    for (Polygon* poly : v->NeighborPolys)
    {
        normal += poly->Normal;
    }

    normal /= v->NeighborPolys.size();
    normal = Vec4::Normalize3(normal);
    normal[3] = 0.0;

    return normal;
}

void Model::CalculateVertexNormals(Geometry* geo)
{
    for (auto& it : geo->Vertices)
    {
        Vertex* v = (Vertex*)it.second;
        if (v->NormalID != -1)
            continue;
        
        // Get calulcated normal new index
        int index = VertexNormals.size();
        // Calculate normal
        Vec4 normal = CalculateVertexNormal(v);
        // Add normal to normals vector in geo
        VertexNormals.push_back(normal);
        // Save index in vertex
        v->NormalID = index;

    }
}

 void Model::AddVertexToPoly(Geometry* geo, Polygon* poly, int posID, int normID, int texID)
 {
    Vertex* vert = NULL;
    // Add it's poly to it's neighbor polys vector
    // and Vertex to Vertices unordered map
    auto it = geo->Vertices.find(posID);
    if (it != geo->Vertices.end())
    {
        vert = geo->Vertices[posID];
        vert->NeighborPolys.push_back(poly);
    }
    else
    {
        vert = new Vertex(posID, texID, normID);
        vert->NeighborPolys.push_back(poly);
        geo->Vertices[posID] = vert;
    }

    // Add vertex to poly
    poly->Vertices.push_back(vert);
 }
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
    // Delete BBox verticies and polygons
    while (BoundingBoxPolygons.size() > 0)
    {
        Polygon* poly = BoundingBoxPolygons.back();
        BoundingBoxPolygons.pop_back();

        for (Vertex* it : poly->Vertices)
            delete it;

        delete poly;
    }

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
            SetMinMaxDimensions(position);
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

            // Calculate center
            poly->Center /= poly->Vertices.size();
            poly->Center[3] = 1.0;

            // Add poly to polygons vector
            geo->Polygons.push_back(poly);
        }
        else
        {
            // Do nothing
        }
    }
    file.close();

    // Add last gemoetry in file
    AddGeometry(geo);

    // Build Bounding Box
    BuildModelBoundingBox();
}

void Model::AddGeometry(Geometry* geo)
{
    CalculateVertexNormals(geo);
    BuildGeoBoundingBox(geo);

    geos.push_back(geo);
}

const std::vector<Geometry*>& Model::GetGeometries() const
{
    return geos;
}

const Mat4& Model::GetObjectToWorldTransform() const
{
    return objectToWorld;
}

const Mat4& Model::GetViewTransform() const
{
    return viewTransform;
}

void Model::Translate(const Mat4& T, int space)
{
    
    switch (space)
    {
        case ID_SPACE_OBJECT:
            objectToWorld = T * objectToWorld;
            break;
        case ID_SPACE_WORLD:
            objectToWorld = objectToWorld * T;
            break;
        default:
            viewTransform = T * viewTransform;
            break;
    }
}

void Model::Rotate(const Mat4& R, int space)
{
    switch (space)
    {
        case ID_SPACE_OBJECT:
            objectToWorld = R * objectToWorld;
            break;
        case ID_SPACE_WORLD:
            objectToWorld = objectToWorld * R;
            break;
        default:
            viewTransform = R * viewTransform;
            break;
    }
}

void Model::Scale(const Mat4& S, int space)
{
    switch (space)
    {
        case ID_SPACE_OBJECT:
            objectToWorld = S * objectToWorld;
            break;
        case ID_SPACE_WORLD:
            objectToWorld = objectToWorld * S;
            break;
        default:
            viewTransform = S * viewTransform;
            break;
    }
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
    }
    else
    {
        vert = new Vertex(posID, texID, normID);
        geo->Vertices[posID] = vert;
    }
    vert->NeighborPolys.push_back(poly);

    // Add vertex to poly
    poly->Vertices.push_back(vert);

    // Add the vertex position to the center calculation
    poly->Center += VertexPositions[posID];

    // Set maximum and minimum dimensions
    if (geo->Vertices.size() == 1)
    {
        geo->MaxDimensions = VertexPositions[posID];
        geo->MinDimensions = VertexPositions[posID];
    }
    else
    {
        for (int i = 0; i < 3; i++)
        {
            geo->MaxDimensions[i] = ((VertexPositions[posID])[i] > geo->MaxDimensions[i]) ? 
                (VertexPositions[posID])[i] : geo->MaxDimensions[i];
            geo->MinDimensions[i] = ((VertexPositions[posID])[i] < geo->MinDimensions[i]) ? 
                (VertexPositions[posID])[i] : geo->MinDimensions[i];
        }
    }
}

void Model::BuildGeoBoundingBox(Geometry* geo)
{
    if (geo == NULL)
        return;
    
    geo->BoundingBoxPolygons = BuildBoundingBox(geo->MinDimensions, geo->MaxDimensions);
}

void Model::BuildModelBoundingBox()
{
    BoundingBoxPolygons = BuildBoundingBox(minDimensions, maxDimensions);
    LOG_INFO("Model dimensions: ({0}, {1}, {2}).", 
        abs(maxDimensions[0] - minDimensions[0]) / 2.0,
        abs(maxDimensions[1] - minDimensions[1]) / 2.0,
        abs(maxDimensions[2] - minDimensions[2]) / 2.0);
}

void Model::SetMinMaxDimensions(const Vec4& vertPos)
{
    if (VertexPositions.size() == 1)
    {
        for (int i = 0; i < 3; i++)
        {
            maxDimensions[i] = vertPos[i];
            minDimensions[i] = vertPos[i];
        }
    }
    else
    {
        for (int i = 0; i < 3; i++)
        {
            maxDimensions[i] = (vertPos[i] > maxDimensions[i]) ? vertPos[i] : maxDimensions[i];
            minDimensions[i] = (vertPos[i] < minDimensions[i]) ? vertPos[i] : minDimensions[i];
        }
    }
}

std::vector<Polygon*> Model::BuildBoundingBox(const Vec4& minDimensions, const Vec4& maxDimensions)
{
    std::vector<Polygon*> polygons;

    int start = VertexPositions.size();
    VertexPositions.push_back(Vec4(minDimensions[0], minDimensions[1], maxDimensions[2])); // front bottom left
    VertexPositions.push_back(Vec4(minDimensions[0], maxDimensions[1], maxDimensions[2])); // front top left
    VertexPositions.push_back(Vec4(maxDimensions[0], maxDimensions[1], maxDimensions[2])); // front top right
    VertexPositions.push_back(Vec4(maxDimensions[0], minDimensions[1], maxDimensions[2])); // front bottom right
    VertexPositions.push_back(Vec4(minDimensions[0], minDimensions[1], minDimensions[2])); // back bottom left
    VertexPositions.push_back(Vec4(minDimensions[0], maxDimensions[1], minDimensions[2])); // back top left
    VertexPositions.push_back(Vec4(maxDimensions[0], maxDimensions[1], minDimensions[2])); // back top right
    VertexPositions.push_back(Vec4(maxDimensions[0], minDimensions[1], minDimensions[2])); // back bottom right

    // Front face polygon
    Polygon* front = new Polygon();
    front->Vertices.push_back(new Vertex(start));
    front->Vertices.push_back(new Vertex(start + 1));
    front->Vertices.push_back(new Vertex(start + 2));
    front->Vertices.push_back(new Vertex(start + 3));
    front->Normal = Vec4(0.0, 0.0, 1.0);
    front->Center = Vec4((maxDimensions[0] - minDimensions[0]) / 2.0,
        (maxDimensions[1] - minDimensions[1]) / 2.0, maxDimensions[2]);

    // Back face Polygon
    Polygon* back = new Polygon();
    back->Vertices.push_back(new Vertex(start + 4));
    back->Vertices.push_back(new Vertex(start + 5));
    back->Vertices.push_back(new Vertex(start + 6));
    back->Vertices.push_back(new Vertex(start + 7));
    back->Normal = Vec4(0.0, 0.0, -1.0);
    back->Center = Vec4((maxDimensions[0] - minDimensions[0]) / 2.0,
        (maxDimensions[1] - minDimensions[1]) / 2.0, minDimensions[2]);

    // Left face Polygon
    Polygon* left = new Polygon();
    left->Vertices.push_back(new Vertex(start + 4));
    left->Vertices.push_back(new Vertex(start + 5));
    left->Vertices.push_back(new Vertex(start + 1));
    left->Vertices.push_back(new Vertex(start));
    left->Normal = Vec4(-1.0, 0.0, 0.0);
    left->Center = Vec4(minDimensions[0], (maxDimensions[1] - minDimensions[1]) / 2.0, 
        (maxDimensions[2] - minDimensions[2]) / 2.0);

    // Right face Polygon
    Polygon* right = new Polygon();
    right->Vertices.push_back(new Vertex(start + 7));
    right->Vertices.push_back(new Vertex(start + 6));
    right->Vertices.push_back(new Vertex(start + 2));
    right->Vertices.push_back(new Vertex(start + 3));
    right->Normal = Vec4(1.0, 0.0, 0.0);
    right->Center = Vec4(maxDimensions[0], (maxDimensions[1] - minDimensions[1]) / 2.0, 
        (maxDimensions[2] - minDimensions[2]) / 2.0);

    // Top face Polygon
    Polygon* top = new Polygon();
    top->Vertices.push_back(new Vertex(start + 1));
    top->Vertices.push_back(new Vertex(start + 5));
    top->Vertices.push_back(new Vertex(start + 6));
    top->Vertices.push_back(new Vertex(start + 2));
    top->Normal = Vec4(0.0, 1.0, 0.0);
    top->Center = Vec4((maxDimensions[0] - minDimensions[0]) / 2.0, maxDimensions[1], 
        (maxDimensions[2] - minDimensions[2]) / 2.0);

    // Bottom face Polygon
    Polygon* bottom = new Polygon();
    bottom->Vertices.push_back(new Vertex(start));
    bottom->Vertices.push_back(new Vertex(start + 4));
    bottom->Vertices.push_back(new Vertex(start + 7));
    bottom->Vertices.push_back(new Vertex(start + 3));
    bottom->Normal = Vec4(0.0, -1.0, 0.0);
    bottom->Center = Vec4((maxDimensions[0] - minDimensions[0]) / 2.0, minDimensions[1], 
        (maxDimensions[2] - minDimensions[2]) / 2.0);
    
    polygons.push_back(front);
    polygons.push_back(back);
    polygons.push_back(left);
    polygons.push_back(right);
    polygons.push_back(top);
    polygons.push_back(bottom);

    return polygons;
}
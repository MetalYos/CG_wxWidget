#include "Scene.h"

Scene::Scene()
{
    camera = new Camera();
    camera->LookAt(Vec4(0.0, 0.0, -10.0), Vec4(0.0, 0.0, 0.0), Vec4(0.0, -1.0, 0.0));
}

Scene::~Scene()
{
    delete camera;
}

bool Scene::LoadModelFromFile(const std::string& filename)
{
    // Load model
    Model* model = new Model();
    model->LoadFromFile(filename);
    models.push_back(model);

    // Frame camera on model

    return true;
}

std::vector<Model*>& Scene::GetModels()
{
    return models;
}

Camera* Scene::GetCamera()
{
    return camera;
}

Renderer& Scene::GetRenderer()
{
    return renderer;
}

void Scene::Resized(int width, int height)
{
    // Set Renderer size
    renderer.SetWidth(width);
    renderer.SetHeight(height);

    // Update Projection matricies
    bool isPerspective = camera->IsPerpsective();
    LOG_TRACE("Scene: Resized: isPerpsective = {0}", isPerspective);
    PerspectiveParameters perspParams = camera->GetPerspectiveParameters();
    camera->SetPerspective(perspParams.FOV, renderer.GetAspectRatio(), 
        perspParams.Near, perspParams.Far);

    OrthographicParameters orthoParams = camera->GetOrthographicParameters();
    double orthoWidth = abs(orthoParams.Right - orthoParams.Left);
    LOG_TRACE("Scene: Resized: orthoWidth = {0}", orthoWidth);
    camera->SetOrthographic(orthoWidth, renderer.GetAspectRatio(), 
        orthoParams.Near, orthoParams.Far);
    
    if (isPerspective)
    {
        camera->SwitchToProjection(true);
        LOG_TRACE("Scene: Resized: Switched to Orthographic");
    }
}

void Scene::Draw()
{
    // Draw Background
    wxColour blackCol;
    blackCol.Set(wxT("#000000"));
    renderer.DrawBackgeound(blackCol);

    wxColour col;
    col.Set(wxT("#0000FF"));

    Mat4 camTransform = camera->GetTransform();
    Mat4 projection = camera->GetProjection();

    for (Model* model : models)
    {
        Mat4 modelTransform = model->GetTransform();

        auto geos = model->GetGeometries();
        for (Geometry* geo : geos)
        {
            for (Polygon* poly : geo->Polygons)
            {
                DrawPolygon(poly, model, modelTransform, camTransform, projection, col);
            }
        }
    }
}

void Scene::DrawPolygon(Polygon* poly, Model* model, const Mat4& modelTransform, 
    const Mat4& camTransform, const Mat4& projection, const wxColour& color)
{
    for (unsigned int i = 0; i < poly->Vertices.size(); i++)
    {
        // Get vertices positions in object space
        Vec4 pos1 = model->VertexPositions[poly->Vertices[i]->PositionID];
        Vec4 pos2 = model->VertexPositions[poly->Vertices[(i + 1) % poly->Vertices.size()]->PositionID];

        // Transform vertices from object space to NDC
        pos1 = pos1 * modelTransform * camTransform * projection;
        pos2 = pos2 * modelTransform * camTransform * projection;

        // Divide by w
        pos1 /= pos1[3];
        pos2 /= pos2[3];

        // Transform to screen space
        pos1 = pos1 * renderer.GetToScreenMatrix();
        pos2 = pos2 * renderer.GetToScreenMatrix();

        Point pix1((int)pos1[0], (int)pos1[1]);
        Point pix2((int)pos2[0], (int)pos2[1]);
        
        // Draw Edge
        renderer.DrawLine(pix1, pix2, color);
    }
}

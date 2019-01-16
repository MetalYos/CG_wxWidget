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
    bool isPerspective = camera->IsPerspective();
    PerspectiveParameters perspParams = camera->GetPerspectiveParameters();
    camera->SetPerspective(perspParams.FOV, renderer.GetAspectRatio(), 
        perspParams.Near, perspParams.Far);

    OrthographicParameters orthoParams = camera->GetOrthographicParameters();
    double orthoWidth = abs(orthoParams.Right - orthoParams.Left);
    camera->SetOrthographic(orthoWidth, renderer.GetAspectRatio(), 
        orthoParams.Near, orthoParams.Far);
    
    if (isPerspective)
        camera->SwitchToProjection(true);
}

void Scene::DrawBackground()
{
    if (Settings::IsBackgroundOn)
    {
        renderer.DrawBackgroundImage(Settings::BackgroundImage, Settings::IsBackgroundStretched,
            (ImageInterpolationType)Settings::BackgroundInterpolation);
    }
    else
    {
        Vec4 bgColor(0.0, 0.0, 0.0);
        renderer.DrawBackground(bgColor);
    }
}

void Scene::Draw()
{
    DrawBackground();

    Mat4 camTransform = camera->GetWorldToViewTransform();
    Mat4 projection = camera->GetProjection();

    for (Model* model : models)
    {
        Vec4 colorVec = Vec4(255, 255, 255);
        wxColour color((unsigned int)colorVec[0], (unsigned int)colorVec[1], 
        (unsigned int)colorVec[2]);

        Mat4 objectToWorld = model->GetObjectToWorldTransform();
        Mat4 viewTransform = model->GetViewTransform();

        if (Settings::IsPlayingAnimation)
        {
            const Frame* currentFrame = anim.GetCurrentFrame();
            objectToWorld = currentFrame->ObjectToWorldTransform;
            viewTransform = currentFrame->ViewTransform;
        }

        DrawModel(model, objectToWorld, camTransform, viewTransform, projection, color);
    }
}

void Scene::DrawEdge(const Vec4& p0, const Vec4& p1, const Mat4& objectToWorld, const Mat4& camTransform,
    const Mat4& viewTransform, const Mat4& projection, const wxColour& color, int thickness)
{
    // Get vertices positions in object space
    Vec4 pos1 = p0;
    Vec4 pos2 = p1;

    // Transform vertices from object space to NDC
    pos1 = pos1 * objectToWorld * camTransform * viewTransform * projection;
    pos2 = pos2 * objectToWorld * camTransform * viewTransform * projection;

    // Divide by w
    pos1 /= pos1[3];
    pos2 /= pos2[3];

    // Transform to screen space
    pos1 = pos1 * renderer.GetToScreenMatrix();
    pos2 = pos2 * renderer.GetToScreenMatrix();

    Point pix1((int)pos1[0], (int)pos1[1]);
    Point pix2((int)pos2[0], (int)pos2[1]);
    
    // Draw Edge
    renderer.DrawLine(pix1, pix2, color, thickness);
}

void Scene::DrawPolygon(Polygon* poly, Model* model, const Mat4& objectToWorld, 
    const Mat4& camTransform, const Mat4& viewTransform, const Mat4& projection, const wxColour& color)
{
    if (Settings::IsBackFaceCullingEnabled && 
        IsBackFace(poly, objectToWorld, camTransform, viewTransform, projection))
        return;

    for (unsigned int i = 0; i < poly->Vertices.size(); i++)
    {
        // Get vertices positions in object space
        Vec4 pos1 = model->VertexPositions[poly->Vertices[i]->PositionID];
        Vec4 pos2 = model->VertexPositions[poly->Vertices[(i + 1) % poly->Vertices.size()]->PositionID];

        DrawEdge(pos1, pos2, objectToWorld, camTransform, viewTransform, projection, color);
    }
}

void Scene::DrawModel(Model* model, const Mat4& objectToWorld, const Mat4& camTransform, 
    const Mat4& viewTransform, const Mat4& projection, const wxColour& color)
{
    auto geos = model->GetGeometries();
    wxColour bbColor(255, 0, 0);

    for (Geometry* geo : geos)
    {
        for (Polygon* poly : geo->Polygons)
        {
            DrawPolygon(poly, model, objectToWorld, camTransform, viewTransform, projection, color);
        }

        if (Settings::IsBoundingBoxOn && Settings::IsBoundingBoxGeo)
        {
            for (Polygon* poly : geo->BoundingBoxPolygons)
            {
                DrawPolygon(poly, model, objectToWorld, camTransform, viewTransform, projection, bbColor);
            }
        }
    }

    if (Settings::IsBoundingBoxOn && !Settings::IsBoundingBoxGeo)
    {
        for (Polygon* poly : model->BoundingBoxPolygons)
        {
            DrawPolygon(poly, model, objectToWorld, camTransform, viewTransform, projection, bbColor);
        } 
    }

    DrawOrigin(Vec4(0.0, 0.0, 0.0), objectToWorld, camTransform, viewTransform, projection);
}

void Scene::DrawOrigin(const Vec4& origin, const Mat4& objectToWorld, 
    const Mat4& camTransform, const Mat4& viewTransform, const Mat4& projection)
{
    double sizeFactor = 1.0;
    // Draw X axis
    Vec4 colorVec(255, 0, 0);
    wxColour color((unsigned int)colorVec[0], (unsigned int)colorVec[1], 
        (unsigned int)colorVec[2]);

    Vec4 pos1 = origin;
    Vec4 pos2 = origin + Vec4(1.0, 0.0, 0.0) * sizeFactor;

    DrawEdge(pos1, pos2, objectToWorld, camTransform, viewTransform, projection, color, 1);

    // Draw Y axis
    colorVec = Vec4(0, 255, 0);
    color = wxColour((unsigned int)colorVec[0], (unsigned int)colorVec[1], 
        (unsigned int)colorVec[2]);

    pos2 = origin + Vec4(0.0, 1.0, 0.0) * sizeFactor;

    DrawEdge(pos1, pos2, objectToWorld, camTransform, viewTransform, projection, color, 1);

    // Draw Z axis
    colorVec = Vec4(0, 0, 255);
    color = wxColour((unsigned int)colorVec[0], (unsigned int)colorVec[1], 
        (unsigned int)colorVec[2]);

    pos2 = origin + Vec4(0.0, 0.0, 1.0) * sizeFactor;

    DrawEdge(pos1, pos2, objectToWorld, camTransform, viewTransform, projection, color, 1);
}

bool Scene::IsBackFace(Polygon* p, const Mat4& objectToWorld, const Mat4& camTransform,
    const Mat4& viewTransform, const Mat4& projection)
{
    Vec4 normal = p->Normal;
    normal[3] = 0.0;

    // Transform normal and poly center to view space
    normal = normal * objectToWorld * camTransform * viewTransform;
    Vec4 center = p->Center * objectToWorld * camTransform * viewTransform;

    if (camera->IsPerspective())
    {
        if (Vec4::Dot3(center, normal) > 0)
            return true;
        return false;
    }
        normal = Vec4::Normalize3(normal * projection);
	    return normal[2] < 0;
}

void Scene::StartRecordingAnimation()
{
    anim.ClearAnimation();
}

void Scene::AddKeyFrame(double timeDiff, const Vec4& offsets)
{
    if (models.size() == 0)
        return;

    Frame* frame = new Frame();
    frame->ObjectToWorldTransform = models.back()->GetObjectToWorldTransform();
    frame->ViewTransform = models.back()->GetViewTransform();
    frame->Action[0] = (Settings::SelectedAction == ID_ACTION_TRANSLATE);
    frame->Action[1] = (Settings::SelectedAction == ID_ACTION_SCALE);
    frame->Action[2] = (Settings::SelectedAction == ID_ACTION_ROTATE);
    frame->Space = Settings::SelectedSpace;
    if (Settings::SelectedAction == ID_ACTION_TRANSLATE)
    {
        for (int i = 0; i < 3; i++)
            frame->Translation[i] = (Settings::SelectedAxis[i]) ? offsets[0] : 0.0;
    }
    else if (Settings::SelectedAction == ID_ACTION_SCALE)
    {
        double scaleFactor = (offsets[1] < Settings::MinScaleFactor) ? 
            Settings::MinScaleFactor : offsets[1];
        for (int i = 0; i < 3; i++)
            frame->Scale[i] = (Settings::SelectedAxis[i]) ? scaleFactor : 1.0;
    }
    else
    {
        for (int i = 0; i < 3; i++)
            frame->Rotation[i] = (Settings::SelectedAxis[i]) ? offsets[2] : 0.0;
    }

    if (anim.GetFrame(0) == NULL)
        frame->FrameNum = 0;
    else
        frame->FrameNum = anim.GetLastFrameNumber() + 
            (int)(timeDiff * (double)Settings::FramesPerSeconds);
    frame->OriginalFrame = frame->FrameNum;
    
    LOG_TRACE("Scene::AddKeyFrame: Added KeyFrame at frame: {0}", frame->FrameNum);
    anim.AddKeyFrame(frame);
}

bool Scene::PlayAnimation()
{
    if ((models.size() == 0) || anim.GetLastFrameNumber() < 0)
        return false;

    const Frame* frame = anim.GetCurrentFrame();
    if ((frame == NULL) || (frame->FrameNum == anim.GetLastFrameNumber()))
    {
        // Animation ended
        anim.ResetAnimation();
        Settings::IsPlayingAnimation = false;
        LOG_TRACE("Scene::PlayAnimation: Finished Playing Animation.");
        return false;
    }
    
    anim.StepToNextFrame();
    return true;
}

void Scene::IncreasePlaybackSpeed(double percentage)
{
    anim.IncreasePlaybackSpeed(percentage);
}

void Scene::DecreasePlaybackSpeed(double percentage)
{
    anim.DecreasePlaybackSpeed(percentage);
}

void Scene::NormalPlaybackSpeed()
{
    anim.NormalPlaybackSpeed();
}

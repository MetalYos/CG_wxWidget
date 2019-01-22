#include "Scene.h"

Scene::Scene()
    : selectedModelIndex(-1)
{
    camera = new Camera();
}

Scene::~Scene()
{
    DeleteModels();
    delete camera;
}

void Scene::ClearScene()
{
    DeleteModels();
    delete camera;

    camera = new Camera();
}

bool Scene::LoadModelFromFile(const std::string& filename)
{
    // Load model
    Model* model = new Model();
    model->LoadFromFile(filename);
    models.push_back(model);
    selectedModelIndex = models.size() - 1;

    // Frame camera on model
    FrameCameraOnModel(model);

    return true;
}

std::vector<Model*>& Scene::GetModels()
{
    return models;
}

Model* Scene::GetSelectedModel()
{
    if (selectedModelIndex < 0)
        return nullptr;
    
    return models[selectedModelIndex];
}

void Scene::SelectNextModel()
{
    selectedModelIndex++;
    if (selectedModelIndex >= (int)models.size()) selectedModelIndex = 0;
}

void Scene::SelectPreviousModel()
{
    selectedModelIndex--;
    if (selectedModelIndex < 0) selectedModelIndex = models.size() - 1;
}

void Scene::ClearModelSelection()
{
    selectedModelIndex = -1;
}

void Scene::SetMaterial(const Material& material)
{
    if (selectedModelIndex < 0)
        return;
    
    Material* mat = GetSelectedModel()->GetMaterial();
    mat->Color = material.Color;
    mat->Ka = material.Ka;
    mat->Kd = material.Kd;
    mat->Ks = material.Ks;
    mat->Specular = material.Specular;
}

Camera* Scene::GetCamera()
{
    return camera;
}

Renderer& Scene::GetRenderer()
{
    return renderer;
}

void Scene::FrameCameraOnModel(Model* model, bool newModel)
{
    Vec4 dimensions = model->GetModelDimensions() * model->GetObjectToWorldTransform();
    Vec4 center = model->GetModelBBoxCenter() * model->GetObjectToWorldTransform();
    double maxDim = MaxDbl(MaxDbl(dimensions[0], dimensions[1]), dimensions[2]);
    LOG_INFO("Model BBox Center: ({0}, {1}, {2})", center[0], center[1], center[2]);

    // Set Camera position
    double radius = maxDim / 2.0;
    double f = sin(ToRadians(camera->GetPerspectiveParameters().FOV / 2.0));
    if (dimensions[0] > dimensions[1])
    {
        f = sin(ToRadians(renderer.GetAspectRatio() * camera->GetPerspectiveParameters().FOV / 2.0));
    }
    double zPos = abs(radius / f) * Settings::CameraFrameOffset;
    zPos *= 1.2;
    if (newModel && (zPos > abs(camera->GetCameraParameters().Eye[2])))
    {
        Vec4 eye = center - Vec4(0.0, 0.0, -zPos);
        camera->LookAt(eye, center, Vec4(0.0, -1.0, 0.0));
        LOG_INFO("Moved camera to: ({0}, {1}, {2})", eye[0], eye[1], eye[2]);

        // Set Orthographic Projection
        bool isPerspective = camera->IsPerspective();
        double orthoWidth = (dimensions[0] > dimensions[1]) ? 
            dimensions[0] : renderer.GetAspectRatio() * dimensions[1];
        orthoWidth *= Settings::CameraFrameOffset;
        camera->SetOrthographic(orthoWidth, renderer.GetAspectRatio(), 1.0, 1000.0);
        LOG_INFO("Set Orthographic Projection Width to: {0}", orthoWidth);

        // Switch camera to the correct projection (the one that is selected)
        camera->SwitchToProjection(isPerspective);
    }

    originalCamParams = camera->GetCameraParameters();
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

    renderer.InitZBuffer();
    for (Model* model : models)
    {
        Vec4 colorVec = model->GetMaterial()->Color;
        if (model == GetSelectedModel())
            colorVec = Vec4(255, 255, 0);
        
        wxColour color((unsigned int)colorVec[0], (unsigned int)colorVec[1], 
        (unsigned int)colorVec[2]);

        Mat4 objectToWorld = model->GetObjectToWorldTransform();
        Mat4 viewTransform = model->GetViewTransform();

        if (Settings::IsPlayingAnimation)
        {
            const Frame* currentFrame = model->GetAnimation()->GetCurrentFrame();

            if (currentFrame != nullptr)
            {
                objectToWorld = currentFrame->ObjectToWorldTransform;
                viewTransform = currentFrame->ViewTransform;
            }
        }

        DrawModel(model, objectToWorld, camTransform, viewTransform, projection, color);
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
            if (Settings::IsBackFaceCullingEnabled && 
                IsBackFace(poly, objectToWorld, camTransform, viewTransform, projection))
                continue;
            
            renderer.DrawPolygon(poly, model, objectToWorld, camTransform, viewTransform, projection, color);
            //renderer.FillPolygon(model, poly, camTransform, projection, model->GetMaterial()->Color);
        }

        if (Settings::IsBoundingBoxOn && Settings::IsBoundingBoxGeo)
        {
            for (Polygon* poly : geo->BoundingBoxPolygons)
            {
                renderer.DrawPolygon(poly, model, objectToWorld, camTransform, viewTransform, projection, bbColor);
            }
        }
    }

    if (Settings::IsBoundingBoxOn && !Settings::IsBoundingBoxGeo)
    {
        for (Polygon* poly : model->BoundingBoxPolygons)
        {
            renderer.DrawPolygon(poly, model, objectToWorld, camTransform, viewTransform, projection, bbColor);
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
    renderer.DrawEdge(pos1, pos2, objectToWorld, camTransform, viewTransform, projection, color, 1);

    // Draw Y axis
    colorVec = Vec4(0, 255, 0);
    color = wxColour((unsigned int)colorVec[0], (unsigned int)colorVec[1], 
        (unsigned int)colorVec[2]);
    pos2 = origin + Vec4(0.0, 1.0, 0.0) * sizeFactor;
    renderer.DrawEdge(pos1, pos2, objectToWorld, camTransform, viewTransform, projection, color, 1);

    // Draw Z axis
    colorVec = Vec4(0, 0, 255);
    color = wxColour((unsigned int)colorVec[0], (unsigned int)colorVec[1], 
        (unsigned int)colorVec[2]);
    pos2 = origin + Vec4(0.0, 0.0, 1.0) * sizeFactor;
    renderer.DrawEdge(pos1, pos2, objectToWorld, camTransform, viewTransform, projection, color, 1);
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
    for (Model* model : models)
        model->GetAnimation()->ClearAnimation();
}

void Scene::AddKeyFrame(double timeDiff, const Vec4& offsets)
{
    if (models.size() == 0)
        return;

    Frame* frame = new Frame();
    frame->ObjectToWorldTransform = GetSelectedModel()->GetObjectToWorldTransform();
    frame->ViewTransform = GetSelectedModel()->GetViewTransform();
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

    Animation* anim = GetSelectedModel()->GetAnimation();
    if (anim->GetFrame(0) == NULL)
        frame->FrameNum = 0;
    else
        frame->FrameNum = anim->GetLastFrameNumber() + 
            (int)(timeDiff * (double)Settings::FramesPerSeconds);
    frame->OriginalFrame = frame->FrameNum;
    
    LOG_TRACE("Scene::AddKeyFrame: Added KeyFrame at frame: {0}", frame->FrameNum);
    anim->AddKeyFrame(frame);
}

bool Scene::PlayAnimation()
{
    if (models.size() == 0)
        return false;

    bool isSomeonePlaying = false;
    for (Model* model : models)
    {
        Animation* anim = model->GetAnimation();
        const Frame* frame = anim->GetCurrentFrame();
        isSomeonePlaying = ((frame != nullptr) && (frame->FrameNum < anim->GetLastFrameNumber())) || isSomeonePlaying;
    }

    //if ((frame == NULL) || (frame->FrameNum == anim.GetLastFrameNumber()))
    if (!isSomeonePlaying)
    {
        // Animation ended
        for (Model* model : models)
            model->GetAnimation()->ResetAnimation();
        
        Settings::IsPlayingAnimation = false;
        LOG_TRACE("Scene::PlayAnimation: Finished Playing Animation.");
        return false;
    }
    
    for (Model* model : models)
        model->GetAnimation()->StepToNextFrame();
    
    return true;
}

void Scene::IncreasePlaybackSpeed(double percentage)
{
    for (Model* model : models)
        model->GetAnimation()->IncreasePlaybackSpeed(percentage);
}

void Scene::DecreasePlaybackSpeed(double percentage)
{
    for (Model* model : models)
        model->GetAnimation()->DecreasePlaybackSpeed(percentage);
}

void Scene::NormalPlaybackSpeed()
{
    for (Model* model : models)
        model->GetAnimation()->NormalPlaybackSpeed();
}

void Scene::DeleteModels()
{
    while (models.size() > 0)
    {
        Model* model = models.back();
        models.pop_back();
        delete model;
    }
}

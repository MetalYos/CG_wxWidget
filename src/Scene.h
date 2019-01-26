#pragma once

#include "pch.h"
#include "Renderer.h"
#include "Model.h"
#include "Camera.h"
#include "Animation.h"

#define SCENE Scene::GetInstance()

class Scene
{
    public:
        static Scene& GetInstance()
        {
            static Scene instance;
            return instance;
        }
        ~Scene();

        Scene(Scene const&) = delete;
        void operator=(Scene const&) = delete;
    
        void ClearScene();

        // Models Methods
        bool LoadModelFromFile(const std::string& filename);
        std::vector<Model*>& GetModels();
        Model* GetSelectedModel();
        void SelectNextModel();
        void SelectPreviousModel();
        void ClearModelSelection();
        void SetMaterial(const Material& material);
        void SelectModel(const Vec4& mousePos, bool useBBox = false);

        Camera* GetCamera();
        Renderer& GetRenderer();

        // Camera Methods
        void FrameCameraOnModel(Model* model, bool newModel = true);

        // Animation methods
        void StartRecordingAnimation();
        void AddKeyFrame(double timeDiff = 0.0, const Vec4& offsets = Vec4(0.0, 1.0, 0.0));
        bool PlayAnimation();
        void IncreasePlaybackSpeed(double percentage);
        void DecreasePlaybackSpeed(double percentage);
        void NormalPlaybackSpeed();

        void Resized(int width, int height);
        void Draw();

    private:
        Scene();

        void DrawBackground();
        void DrawModel(Model* model, const Mat4& objectToWorld, const Mat4& camTransform, 
            const Mat4& viewTransform, const Mat4& projection, const wxColour& color);
        void DrawOrigin(const Vec4& origin, const Mat4& objectToWorld, const Mat4& camTransform, 
            const Mat4& viewTransform, const Mat4& projection);
        bool IsBackFace(Polygon* p, const Mat4& objectToWorld, const Mat4& camTransform,
            const Mat4& viewTransform, const Mat4& projection);
        void DeleteModels();
        void selectModelPoly(const Vec4& mousePos);
        void selectModelBBox(const Vec4& mousePos);

    private:
        std::vector<Model*> models;
        Camera* camera;
        Renderer renderer;
        int selectedModelIndex;

        CameraParameters originalCamParams;
};
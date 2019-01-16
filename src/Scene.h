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

        bool LoadModelFromFile(const std::string& filename);

        std::vector<Model*>& GetModels();
        Camera* GetCamera();
        Renderer& GetRenderer();

        // Camera Methods
        void FrameCameraOnModel(Model* model);

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
        void DrawEdge(const Vec4& p0, const Vec4& p1, const Mat4& objectToWorld, const Mat4& camTransform,
            const Mat4& viewTransform, const Mat4& projection, const wxColour& color, int thickness = 0);
        void DrawPolygon(Polygon* poly, Model* model, const Mat4& objectToWorld, 
            const Mat4& camTransform, const Mat4& viewTransform, const Mat4& projection, const wxColour& color);
        void DrawModel(Model* model, const Mat4& objectToWorld, const Mat4& camTransform, 
            const Mat4& viewTransform, const Mat4& projection, const wxColour& color);
        void DrawOrigin(const Vec4& origin, const Mat4& objectToWorld, const Mat4& camTransform, 
            const Mat4& viewTransform, const Mat4& projection);
        bool IsBackFace(Polygon* p, const Mat4& objectToWorld, const Mat4& camTransform,
            const Mat4& viewTransform, const Mat4& projection);

    private:
        std::vector<Model*> models;
        Camera* camera;
        Renderer renderer;
        Animation anim;
};
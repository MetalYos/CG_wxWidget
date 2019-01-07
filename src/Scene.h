#pragma once

#include "pch.h"
#include "Renderer.h"
#include "Model.h"
#include "Camera.h"

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

        void Resized(int width, int height);
        void Draw();

    private:
        Scene();

        void DrawEdge(const Vec4& p0, const Vec4& p1, const Mat4& modelTransform, 
            const Mat4& camTransform, const Mat4& projection, const wxColour& color, int thickness = 0);
        void DrawPolygon(Polygon* poly, Model* model, const Mat4& modelTransform, 
            const Mat4& camTransform, const Mat4& projection, const wxColour& color);
        void DrawOrigin(const Vec4& origin, const Mat4& modelTransform, 
            const Mat4& camTransform, const Mat4& projection);

    private:
        std::vector<Model*> models;
        Camera* camera;
        Renderer renderer;
};
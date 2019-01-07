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

        void DrawPolygon(Polygon* poly, Model* model, const Mat4& modelTransform, 
            const Mat4& camTransform, const Mat4& projection, const wxColour& color);

    private:
        std::vector<Model*> models;
        Camera* camera;
        Renderer renderer;
};
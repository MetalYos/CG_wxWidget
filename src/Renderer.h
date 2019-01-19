#pragma once

#include "pch.h"
#include "Model.h"
#include "RendererStructures.h"

enum ImageInterpolationType
{
    IMG_NEAREST_NEIGHBOUR,
    IMG_BILINEAR
};

class Renderer
{
public:
    Renderer(int width = 0, int height = 0);
    ~Renderer();

    void SetDeviceContext(wxDC* dc);

    void SetWidth(int width);
    void SetHeight(int height);
    int GetWidth() const;
    int GetHeight() const;
    double GetAspectRatio() const;
    const Mat4& GetToScreenMatrix() const;

    void DrawPixel(int x, int y, const wxColour& color, int thickness = 0);
    void DrawLine(const Vec4& p0, const Vec4& p1, const wxColour& color, int thickness = 0);
    void DrawBackground(const Vec4& color);
    void DrawBackgroundImage(const std::string& filename, bool stretch = true,
        ImageInterpolationType interpolation = IMG_NEAREST_NEIGHBOUR);
    void DrawEdge(const Vec4& p0, const Vec4& p1, const Mat4& objectToWorld, const Mat4& camTransform,
        const Mat4& viewTransform, const Mat4& projection, const wxColour& color, int thickness = 0);
    void DrawPolygon(Polygon* poly, Model* model, const Mat4& objectToWorld, 
        const Mat4& camTransform, const Mat4& viewTransform, const Mat4& projection, const wxColour& color);

    void InitZBuffer();
    void FillPolygon(Model* model, Polygon* p, const Mat4& camTransform,
        const Mat4& projection, const Vec4& color);

private:
    void BuildToScreenMatrix();
    Vec4 GetStbColor(unsigned char* data, unsigned numChannels, int x, int y, int width);
    void ScanConvert(std::vector<Edge>& poly, wxColour& color, 
        const Vec4& polyCenter, const Vec4& polyNormal);

private:
    int m_Width;
    int m_Height;
    Mat4 m_ToScreen;

    wxDC* m_DC;
    double* m_ZBuffer;
};
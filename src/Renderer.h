#pragma once

#include "pch.h"

struct Point
{
    int X;
    int Y;

    Point(int x = 0, int y = 0) : X(x), Y(y) {}
};

enum ImageInterpolationType
{
    IMG_NEAREST_NEIGHBOUR,
    IMG_BILINEAR
};

class Renderer
{
public:
    Renderer(int width = 0, int height = 0);

    void SetDeviceContext(wxDC* dc);
    void SetWidth(int width);
    void SetHeight(int height);

    int GetWidth() const;
    int GetHeight() const;
    double GetAspectRatio() const;
    const Mat4& GetToScreenMatrix() const;

    void DrawPixel(const Point& p, const wxColour& color, int thickness = 0);
    void DrawPixel(int x, int y, const wxColour& color, int thickness = 0);
    void DrawLine(const Point& p0, const Point& p1, const wxColour& color, int thickness = 0);
    void DrawBackground(const Vec4& color);
    void DrawBackgroundImage(const std::string& filename, bool stretch = true,
        ImageInterpolationType interpolation = IMG_NEAREST_NEIGHBOUR);

private:
    void BuildToScreenMatrix();
    Vec4 GetStbColor(unsigned char* data, unsigned numChannels, int x, int y, int width);

private:
    int m_Width;
    int m_Height;
    Mat4 m_ToScreen;

    wxDC* m_DC;
};
#pragma once

#include "pch.h"

struct Point
{
    int X;
    int Y;

    Point(int x = 0, int y = 0) : X(x), Y(y) {}
};

class Renderer
{
public:
    Renderer(int width = 0, int height = 0);

    void SetDeviceContext(wxDC* dc);

    void SetHeight(int height);
    int GetHeight() const;

    void SetWidth(int width);
    int GetWidth() const;

    double GetAspectRatio() const;

    const Mat4& GetToScreenMatrix() const;

    void DrawPixel(const Point& p, const wxColour& color, int thickness = 0);
    void DrawLine(const Point& p0, const Point& p1, const wxColour& color, int thickness = 0);
    void DrawBackgeound(const wxColour& color);

private:
    void BuildToScreenMatrix();

private:
    int m_Width;
    int m_Height;
    Mat4 m_ToScreen;

    wxDC* m_DC;
};
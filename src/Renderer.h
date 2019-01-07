#pragma once

#include "pch.h"

struct Point
{
    int X;
    int Y;

    Point(int x, int y) : X(x), Y(y) {}
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

    void DrawLine(const Point& p0, const Point& p1, const wxColour& color);
    void DrawBackgeound(const wxColour& color);

private:
    void SetAspectRatio();
    void BuildToScreenMatrix();

private:
    int m_Width;
    int m_Height;
    Mat4 m_ToScreen;

    wxDC* m_DC;
};
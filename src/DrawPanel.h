#pragma once

#include "pch.h"

class DrawPanel : public wxPanel
{
public:
    DrawPanel(wxFrame* parent);

    void OnResize(wxSizeEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnEraseBackground(wxEraseEvent& event);

    // Input handling
    void OnMouseLeftClick(wxMouseEvent& event);
    void OnMouseMiddleClick(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseScroll(wxMouseEvent& event);
    void OnMouseLeftRelease(wxMouseEvent& event);
    void OnMouseMiddleRelease(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);

private:
    wxPoint m_MousePrevPos;
    wxPoint m_MouseClickPos;
    wxPoint m_MouseMiddlePrevPos;
    bool m_IsMouseLeftButtonClicked;
    bool m_IsMouseMiddleButtonClicked;

    clock_t m_MouseDownTicks;
    Vec4 m_Offsets;
};
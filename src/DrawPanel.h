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
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLeftRelease(wxMouseEvent& event);

private:
    wxPoint m_MousePrevPos;
    wxPoint m_MouseClickPos;
    bool m_IsMouseLeftButtonClicked;

    clock_t m_MouseDownTicks;
};
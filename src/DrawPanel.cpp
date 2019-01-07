#include "DrawPanel.h"
#include "Scene.h"

DrawPanel::DrawPanel(wxFrame* parent)
    : wxPanel(parent)
{
    Connect(wxEVT_PAINT, wxPaintEventHandler(DrawPanel::OnPaint));
    Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(DrawPanel::OnEraseBackground));
    Connect(wxEVT_SIZE, wxSizeEventHandler(DrawPanel::OnResize));
    Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(DrawPanel::OnMouseLeftClick));
    Connect(wxEVT_MOTION, wxMouseEventHandler(DrawPanel::OnMouseMove));
    Connect(wxEVT_LEFT_UP, wxMouseEventHandler(DrawPanel::OnMouseLeftRelease));
}

void DrawPanel::OnResize(wxSizeEvent& event)
{
    wxSize size = event.GetSize();
    Scene::GetInstance().Resized(size.GetWidth(), size.GetHeight());
}

void DrawPanel::OnPaint(wxPaintEvent& event)
{
    wxBufferedPaintDC dc(this);
    Scene::GetInstance().GetRenderer().SetDeviceContext(&dc);

    Scene::GetInstance().Draw();
}

void DrawPanel::OnEraseBackground(wxEraseEvent& event)
{
    // Do nothing
}

void DrawPanel::OnMouseLeftClick(wxMouseEvent& event)
{
    m_MousePrevPos = wxGetMousePosition();
    m_MousePrevPos.x = m_MousePrevPos.x - this->GetScreenPosition().x;
    m_MousePrevPos.y = m_MousePrevPos.y - this->GetScreenPosition().y;

    m_IsMouseLeftButtonClicked = true;

    event.Skip();
}

void DrawPanel::OnMouseMove(wxMouseEvent& event)
{
    // Save current mouse position
    wxPoint currentMousePos = wxGetMousePosition();
    currentMousePos.x = currentMousePos.x - this->GetScreenPosition().x;
    currentMousePos.y = currentMousePos.y - this->GetScreenPosition().y;

    // Calculate delta in x mouse movement from previous press
    int dx = m_MousePrevPos.x - currentMousePos.x;

    // Save current mouse pos as previous mouse pos
    m_MousePrevPos = currentMousePos;

    if ((SCENE.GetModels().size() == 0) || !m_IsMouseLeftButtonClicked)
        return;

    if (Settings::SelectedAction == ID_ACTION_TRANSLATE)
    {
        double offset = dx / Settings::MouseSensitivity[0];
        if (Settings::SelectedAxis[0])
        {
            if (Settings::SelectedSpace == ID_SPACE_VIEW)
                SCENE.GetCamera()->Translate(Mat4::Translate(offset, 0.0, 0.0));
            else
                SCENE.GetModels().back()->Translate(Mat4::Translate(offset, 0.0, 0.0), 
                    Settings::SelectedSpace == ID_SPACE_OBJECT);
        }
        if (Settings::SelectedAxis[1])
        {
            if (Settings::SelectedSpace == ID_SPACE_VIEW)
                SCENE.GetCamera()->Translate(Mat4::Translate(0.0, offset, 0.0));
            else
                SCENE.GetModels().back()->Translate(Mat4::Translate(0.0, offset, 0.0), 
                    Settings::SelectedSpace == ID_SPACE_OBJECT);
        }
        if (Settings::SelectedAxis[2])
        {
            if (Settings::SelectedSpace == ID_SPACE_VIEW)
                SCENE.GetCamera()->Translate(Mat4::Translate(0.0, 0.0, offset));
            else
                SCENE.GetModels().back()->Translate(Mat4::Translate(0.0, 0.0, offset), 
                    Settings::SelectedSpace == ID_SPACE_OBJECT);
        }
    }
    else if (Settings::SelectedAction == ID_ACTION_SCALE)
    {
        double scale = 1.0 + dx / Settings::MouseSensitivity[1];
        if (scale < Settings::MinScaleFactor)
            scale = Settings::MinScaleFactor;
        
        if (Settings::SelectedAxis[0])
        {
            if (Settings::SelectedSpace == ID_SPACE_VIEW)
                SCENE.GetCamera()->Scale(Mat4::Scale(scale, 1.0, 1.0));
            else
                SCENE.GetModels().back()->Scale(Mat4::Scale(scale, 1.0, 1.0), 
                    Settings::SelectedSpace == ID_SPACE_OBJECT);
        }
        if (Settings::SelectedAxis[1])
        {
            if (Settings::SelectedSpace == ID_SPACE_VIEW)
                SCENE.GetCamera()->Scale(Mat4::Scale(1.0, scale, 1.0));
            else
                SCENE.GetModels().back()->Scale(Mat4::Scale(1.0, scale, 1.0), 
                    Settings::SelectedSpace == ID_SPACE_OBJECT);
        }
        if (Settings::SelectedAxis[2])
        {
            if (Settings::SelectedSpace == ID_SPACE_VIEW)
                SCENE.GetCamera()->Scale(Mat4::Scale(1.0, 1.0, scale));
            else
                SCENE.GetModels().back()->Scale(Mat4::Scale(1.0, 1.0, scale), 
                    Settings::SelectedSpace == ID_SPACE_OBJECT);
        }
    }
    else if (Settings::SelectedAction == ID_ACTION_ROTATE)
    {
        double angle = dx / Settings::MouseSensitivity[2];
        if (Settings::SelectedAxis[0])
        {
            if (Settings::SelectedSpace == ID_SPACE_VIEW)
                SCENE.GetCamera()->Rotate(Mat4::RotateX(angle));
            else
                SCENE.GetModels().back()->Rotate(Mat4::RotateX(angle), 
                    Settings::SelectedSpace == ID_SPACE_OBJECT);
        }
        if (Settings::SelectedAxis[1])
        {
            if (Settings::SelectedSpace == ID_SPACE_VIEW)
                SCENE.GetCamera()->Rotate(Mat4::RotateY(angle));
            else
                SCENE.GetModels().back()->Rotate(Mat4::RotateY(angle), 
                    Settings::SelectedSpace == ID_SPACE_OBJECT);
        }
        if (Settings::SelectedAxis[2])
        {
            if (Settings::SelectedSpace == ID_SPACE_VIEW)
                SCENE.GetCamera()->Rotate(Mat4::RotateZ(angle));
            else
                SCENE.GetModels().back()->Rotate(Mat4::RotateZ(dx), 
                    Settings::SelectedSpace == ID_SPACE_OBJECT);
        }
    }

    INVALIDATE();
}

void DrawPanel::OnMouseLeftRelease(wxMouseEvent& event)
{
    m_IsMouseLeftButtonClicked = false;
}
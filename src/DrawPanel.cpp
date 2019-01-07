#include "DrawPanel.h"
#include "Scene.h"
#include "Settings.h"

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
    m_MousePosAtClick = wxGetMousePosition();
    m_MousePosAtClick.x = m_MousePosAtClick.x - this->GetScreenPosition().x;
    m_MousePosAtClick.y = m_MousePosAtClick.y - this->GetScreenPosition().y;

    isMouseLeftButtonClicked = true;

    event.Skip();
}

void DrawPanel::OnMouseMove(wxMouseEvent& event)
{
    wxPoint currentMousePos = wxGetMousePosition();
    currentMousePos.x = currentMousePos.x - this->GetScreenPosition().x;
    currentMousePos.y = currentMousePos.y - this->GetScreenPosition().y;

    int dx = m_MousePosAtClick.x - currentMousePos.x;

    double sensitivity = 300.0;
    double angle = dx / sensitivity;

    if (Settings::SelectedAction == ID_ACTION_ROTATE)
    {
        if ((SCENE.GetModels().size() == 0) || !isMouseLeftButtonClicked)
            return;

        if (Settings::SelectedAxis[0])
        {
            if (Settings::SelectedSpace == ID_SPACE_VIEW)
            {
                SCENE.GetCamera()->Rotate(Mat4::RotateX(angle));
            }
            else
            {
                SCENE.GetModels().back()->Rotate(Mat4::RotateX(angle), 
                    Settings::SelectedSpace == ID_SPACE_OBJECT);
            }
        }
        if (Settings::SelectedAxis[1])
        {
            if (Settings::SelectedSpace == ID_SPACE_VIEW)
            {
                SCENE.GetCamera()->Rotate(Mat4::RotateY(angle));
            }
            else
            {
                SCENE.GetModels().back()->Rotate(Mat4::RotateY(angle), 
                    Settings::SelectedSpace == ID_SPACE_OBJECT);
            }
        }
        if (Settings::SelectedAxis[2])
        {
            if (Settings::SelectedSpace == ID_SPACE_VIEW)
            {
                SCENE.GetCamera()->Rotate(Mat4::RotateZ(angle));
            }
            else
            {
                SCENE.GetModels().back()->Rotate(Mat4::RotateZ(dx), 
                    Settings::SelectedSpace == ID_SPACE_OBJECT);
            }
        }

        Refresh();
        Update();
    }
}

void DrawPanel::OnMouseLeftRelease(wxMouseEvent& event)
{
    isMouseLeftButtonClicked = false;
}
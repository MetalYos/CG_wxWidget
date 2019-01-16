#include "DrawPanel.h"
#include "Scene.h"

DrawPanel::DrawPanel(wxFrame* parent)
    : wxPanel(parent), m_IsMouseLeftButtonClicked(false), 
    m_Offsets(Vec4(0.0, 1.0, 0.0))
{
    Connect(wxEVT_PAINT, wxPaintEventHandler(DrawPanel::OnPaint));
    Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(DrawPanel::OnEraseBackground));
    Connect(wxEVT_SIZE, wxSizeEventHandler(DrawPanel::OnResize));
    Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(DrawPanel::OnMouseLeftClick));
    Connect(wxEVT_MOTION, wxMouseEventHandler(DrawPanel::OnMouseMove));
    Connect(wxEVT_LEFT_UP, wxMouseEventHandler(DrawPanel::OnMouseLeftRelease));
    Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(DrawPanel::OnKeyDown));
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

    m_MouseClickPos = m_MousePrevPos;

    m_IsMouseLeftButtonClicked = true;

    if (Settings::IsRecording)
    {
        m_MouseDownTicks = clock();
        m_Offsets = Vec4(0.0, 1.0, 0.0);
    }

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
        double offset = -dx / Settings::MouseSensitivity[0];
        m_Offsets[0] += offset;
        if (Settings::SelectedAxis[0])
        {
            SCENE.GetModels().back()->Translate(Mat4::Translate(offset, 0.0, 0.0), Settings::SelectedSpace);
        }
        if (Settings::SelectedAxis[1])
        {
            SCENE.GetModels().back()->Translate(Mat4::Translate(0.0, offset, 0.0), Settings::SelectedSpace);
        }
        if (Settings::SelectedAxis[2])
        {
            SCENE.GetModels().back()->Translate(Mat4::Translate(0.0, 0.0, offset), Settings::SelectedSpace);
        }
    }
    else if (Settings::SelectedAction == ID_ACTION_SCALE)
    {
        double scale = 1.0 + dx / Settings::MouseSensitivity[1];
        if (scale < Settings::MinScaleFactor)
            scale = Settings::MinScaleFactor;
        m_Offsets[1] *= scale;
        
        if (Settings::SelectedAxis[0])
        {
            SCENE.GetModels().back()->Scale(Mat4::Scale(scale, 1.0, 1.0), Settings::SelectedSpace);
        }
        if (Settings::SelectedAxis[1])
        {
            SCENE.GetModels().back()->Scale(Mat4::Scale(1.0, scale, 1.0), Settings::SelectedSpace);
        }
        if (Settings::SelectedAxis[2])
        {
            SCENE.GetModels().back()->Scale(Mat4::Scale(1.0, 1.0, scale), Settings::SelectedSpace);
        }
    }
    else if (Settings::SelectedAction == ID_ACTION_ROTATE)
    {
        double angle = dx / Settings::MouseSensitivity[2];
        m_Offsets[2] += angle;
        if (Settings::SelectedAxis[0])
        {
            SCENE.GetModels().back()->Rotate(Mat4::RotateX(angle), Settings::SelectedSpace);
        }
        if (Settings::SelectedAxis[1])
        {
            SCENE.GetModels().back()->Rotate(Mat4::RotateY(angle), Settings::SelectedSpace);
        }
        if (Settings::SelectedAxis[2])
        {
            SCENE.GetModels().back()->Rotate(Mat4::RotateZ(angle), Settings::SelectedSpace);
        }
    }

    INVALIDATE();
}

void DrawPanel::OnMouseLeftRelease(wxMouseEvent& event)
{
    m_IsMouseLeftButtonClicked = false;
    if (Settings::IsRecording)
    {
        clock_t ticksDiff = clock() - m_MouseDownTicks;
        double timeDiff = (double)ticksDiff / CLOCKS_PER_SEC;

        Scene::GetInstance().AddKeyFrame(timeDiff, m_Offsets);
    }
}

void DrawPanel::OnKeyDown(wxKeyEvent& event)
{
    int key = (int)event.GetKeyCode();
    LOG_TRACE("DrawPanel::OnKeyDown: Pressed on key {0}", key);

    switch (key)
    {
        case 71: // G key
            Settings::IsBoundingBoxGeo = !Settings::IsBoundingBoxGeo;
            LOG_TRACE("DrawPanel::OnKeyDown: Changed Settings::IsBoundingBoxGeo to {0}", 
                Settings::IsBoundingBoxGeo);
            break;
    }

    INVALIDATE();

    event.Skip();
}

#include "DrawPanel.h"
#include "Scene.h"

DrawPanel::DrawPanel(wxFrame* parent)
    : wxPanel(parent), m_IsMouseLeftButtonClicked(false), 
      m_IsMouseMiddleButtonClicked(false), m_Offsets(Vec4(0.0, 1.0, 0.0))
{
    Connect(wxEVT_PAINT, wxPaintEventHandler(DrawPanel::OnPaint));
    Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(DrawPanel::OnEraseBackground));
    Connect(wxEVT_SIZE, wxSizeEventHandler(DrawPanel::OnResize));
    Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(DrawPanel::OnMouseLeftClick));
    Connect(wxEVT_MIDDLE_DOWN, wxMouseEventHandler(DrawPanel::OnMouseMiddleClick));
    Connect(wxEVT_MOTION, wxMouseEventHandler(DrawPanel::OnMouseMove));
    Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(DrawPanel::OnMouseScroll));
    Connect(wxEVT_LEFT_UP, wxMouseEventHandler(DrawPanel::OnMouseLeftRelease));
    Connect(wxEVT_MIDDLE_UP, wxMouseEventHandler(DrawPanel::OnMouseMiddleRelease));
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

    if (Settings::SelectedAction == ID_ACTION_SELECT)
    {
        SCENE.SelectModel(Vec4(m_MouseClickPos.x, m_MouseClickPos.y, 0.0, 1.0));
        INVALIDATE();
    }

    event.Skip();
}

void DrawPanel::OnMouseMiddleClick(wxMouseEvent& event)
{
    m_MouseMiddlePrevPos = wxGetMousePosition();
    m_MouseMiddlePrevPos.x = m_MouseMiddlePrevPos.x - this->GetScreenPosition().x;
    m_MouseMiddlePrevPos.y = m_MouseMiddlePrevPos.y - this->GetScreenPosition().y;

    m_IsMouseMiddleButtonClicked = true;
}

void DrawPanel::OnMouseMove(wxMouseEvent& event)
{
    // Save current mouse position
    wxPoint currentMousePos = wxGetMousePosition();
    currentMousePos.x = currentMousePos.x - this->GetScreenPosition().x;
    currentMousePos.y = currentMousePos.y - this->GetScreenPosition().y;

    // Calculate delta in x mouse movement from previous press
    int dx = currentMousePos.x - m_MousePrevPos.x;
    int dy = currentMousePos.y - m_MousePrevPos.y;

    // Calculate delta in x and y mouse movements for camera
    int dxCam = currentMousePos.x - m_MouseMiddlePrevPos.x;
    int dyCam = currentMousePos.y - m_MouseMiddlePrevPos.y;

    // Save current mouse pos as previous mouse pos
    m_MousePrevPos = currentMousePos;
    m_MouseMiddlePrevPos = currentMousePos;

    if ((SCENE.GetSelectedModel() != nullptr) && m_IsMouseLeftButtonClicked)
    {
        if (Settings::SelectedAction == ID_ACTION_TRANSLATE)
        {
            double offset = dx / Settings::MouseSensitivity[0];
            m_Offsets[0] += offset;

            Vec4 offsets;
            if (Settings::SelectedAxis[0]) offsets[0] = dx / Settings::MouseSensitivity[0];
            if (Settings::SelectedAxis[1]) offsets[1] = -dy / Settings::MouseSensitivity[0];
            if (Settings::SelectedAxis[2])
            {
                if (Settings::SelectedAxis[0]) offsets[2] = -dy / Settings::MouseSensitivity[0];
                else offsets[2] = dx / Settings::MouseSensitivity[0];
            }

            SCENE.GetSelectedModel()->Translate(Mat4::Translate(offsets), Settings::SelectedSpace);
        }
        else if (Settings::SelectedAction == ID_ACTION_SCALE)
        {
            double scale = 1.0 + dx / Settings::MouseSensitivity[1];
            if (scale < Settings::MinScaleFactor)
                scale = Settings::MinScaleFactor;
            m_Offsets[1] *= scale;
            
            if (Settings::SelectedAxis[0])
            {
                SCENE.GetSelectedModel()->Scale(Mat4::Scale(scale, 1.0, 1.0), Settings::SelectedSpace);
            }
            if (Settings::SelectedAxis[1])
            {
                SCENE.GetSelectedModel()->Scale(Mat4::Scale(1.0, scale, 1.0), Settings::SelectedSpace);
            }
            if (Settings::SelectedAxis[2])
            {
                SCENE.GetSelectedModel()->Scale(Mat4::Scale(1.0, 1.0, scale), Settings::SelectedSpace);
            }
        }
        else if (Settings::SelectedAction == ID_ACTION_ROTATE)
        {
            double angle = dx / Settings::MouseSensitivity[2];
            m_Offsets[2] += angle;
            if (Settings::SelectedAxis[0])
            {
                SCENE.GetSelectedModel()->Rotate(Mat4::RotateX(angle), Settings::SelectedSpace);
            }
            if (Settings::SelectedAxis[1])
            {
                SCENE.GetSelectedModel()->Rotate(Mat4::RotateY(angle), Settings::SelectedSpace);
            }
            if (Settings::SelectedAxis[2])
            {
                SCENE.GetSelectedModel()->Rotate(Mat4::RotateZ(angle), Settings::SelectedSpace);
            }
        }
    }
    if (m_IsMouseMiddleButtonClicked)
    {
        if (wxGetKeyState(WXK_SHIFT))
        {
            SCENE.GetCamera()->PanCamera(dxCam / Settings::MouseSensitivity[0],
                dyCam / Settings::MouseSensitivity[0]);
        }
        else
        {
            SCENE.GetCamera()->RotateCamera(dxCam / Settings::MouseSensitivity[2], 
                dyCam / Settings::MouseSensitivity[2]);
        }
    }

    INVALIDATE();
}

void DrawPanel::OnMouseScroll(wxMouseEvent& event)
{
    int wheelRot = event.GetWheelRotation();
    int wheelDelta = event.GetWheelDelta();

    double wheelOffset = (double)wheelRot / (double)wheelDelta;

    SCENE.GetCamera()->ZoomCamera(wheelOffset);
    INVALIDATE();
}

void DrawPanel::OnMouseLeftRelease(wxMouseEvent& event)
{
    m_IsMouseLeftButtonClicked = false;
    if (Settings::IsRecording)
    {
        clock_t ticksDiff = clock() - m_MouseDownTicks;
        double timeDiff = (double)ticksDiff / CLOCKS_PER_SEC;

        SCENE.AddKeyFrame(timeDiff, m_Offsets);
    }
}

void DrawPanel::OnMouseMiddleRelease(wxMouseEvent& event)
{
    m_IsMouseMiddleButtonClicked = false;
}

void DrawPanel::OnKeyDown(wxKeyEvent& event)
{
    int key = (int)event.GetKeyCode();
    LOG_TRACE("DrawPanel::OnKeyDown: Pressed on key {0}", key);

    switch (key)
    {
        case 71: // G key
            Settings::IsBoundingBoxGeo = !Settings::IsBoundingBoxGeo;
            break;
        case WXK_LEFT:
            SCENE.SelectPreviousModel();
            break;
        case WXK_RIGHT:
            SCENE.SelectNextModel();
            break;
        case WXK_SPACE:
            SCENE.ClearModelSelection();
            break;
    }

    INVALIDATE();

    event.Skip();
}

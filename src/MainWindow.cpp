#include "MainWindow.h"
#include "Scene.h"
#include "CAnimationDialog.h"

MainWindow::MainWindow(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1280, 720))
{
    // Initialize Log
    Log::Init();

    m_MainSizer = new wxBoxSizer(wxVERTICAL);

    CreateMenuBar();
    CreateToolBar();
    CreateDrawingPanel();

    CreateStatusBar(1);
    SetStatusText(wxT("Ready"), 0);

    Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MainWindow::OnUpdateUI));

    SetSizer(m_MainSizer);
    Centre();
}

/**************************** Event Handlers ****************************/
void MainWindow::OnQuit(wxCommandEvent& event)
{
    Close(true);
}

void MainWindow::OnOpenFile(wxCommandEvent& event)
{
    wxFileDialog* fileDialog = new wxFileDialog(this, wxT("Open a model"), wxT(""), wxT(""),
                       wxT("OBJ files (*.obj)|*.obj"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (fileDialog->ShowModal() == wxID_OK)
    {
        m_ModelFileName = fileDialog->GetPath();
        std::string stlstring = std::string(m_ModelFileName.mb_str());
        if (Scene::GetInstance().LoadModelFromFile(stlstring))
        {
            wxMessageDialog *dial = new wxMessageDialog(NULL, 
            wxT("Model was loaded successfully"), wxT("Info"), wxOK | wxICON_INFORMATION);
            dial->ShowModal();
        }
    }
}

void MainWindow::OnUpdateUI(wxUpdateUIEvent& event)
{
    int id = event.GetId();
    switch (id)
    {
        case ID_VIEW_ORTHO:
            OnSwitchToOrthoUI(event);
            break;
        case ID_VIEW_PERSP:
            OnSwitchToPerspUI(event);
            break;
        case ID_VIEW_BACKFACE:
            OnBackFaceCullingUI(event);
            break;
        case ID_VIEW_BACKGROUND_VIEW:
            OnBackgroundViewUI(event);
            break;
        case ID_VIEW_BACKGROUND_STRETCH:
            OnBackgroundStretchUI(event);
            break;
        case ID_VIEW_BACKGROUND_REPEAT:
            OnBackgroundRepeatUI(event);
            break;
        case ID_VIEW_BACKGROUND_INTERPOLATION_LINEAR:
            OnBackgroundInterpolationLinearUI(event);
            break;
        case ID_VIEW_BACKGROUND_INTERPOLATION_BILINEAR:
            OnBackgroundInterpolationBilinearUI(event);
            break;
        case ID_ACTION_TRANSLATE:
            OnSelectTranslateUI(event);
            break;
        case ID_ACTION_SCALE:
            OnSelectScaleUI(event);
            break;
        case ID_ACTION_ROTATE:
            OnSelectRotateUI(event);
            break;
        case ID_AXIS_X:
        case ID_AXIS_Y:
        case ID_AXIS_Z:
            OnChangeAxisUI(event);
            break;
        case ID_SPACE_OBJECT:
            OnSelectObjectSpaceUI(event);
            break;
        case ID_SPACE_WORLD:
            OnSelectWorldSpaceUI(event);
            break;
        case ID_SPACE_VIEW:
            OnSelectViewSpaceUI(event);
            break;
        case ID_ANIMATION_RECORD:
            OnAnimationRecordUI(event);
            break;
    }
}

void MainWindow::OnSwitchProjection(wxCommandEvent& event)
{
    LOG_TRACE("MainWindow::OnSwitchProjection: Switching Projection");
    int id = event.GetId();
    Scene::GetInstance().GetCamera()->SwitchToProjection(id == ID_VIEW_PERSP);
    INVALIDATE();
}

void MainWindow::OnSwitchToOrthoUI(wxUpdateUIEvent& event)
{
    bool isPerspective = Scene::GetInstance().GetCamera()->IsPerspective();
    event.Check(!isPerspective);
}

void MainWindow::OnSwitchToPerspUI(wxUpdateUIEvent& event)
{
    bool isPerspective = Scene::GetInstance().GetCamera()->IsPerspective();
    event.Check(isPerspective);
}

void MainWindow::OnBackFaceCulling(wxCommandEvent& event)
{
    Settings::IsBackFaceCullingEnabled = !Settings::IsBackFaceCullingEnabled;
    INVALIDATE();
}

void MainWindow::OnBackFaceCullingUI(wxUpdateUIEvent& event)
{
    event.Check(Settings::IsBackFaceCullingEnabled);
}

void MainWindow::OnBackgroundOpen(wxCommandEvent& event)
{
    wxFileDialog* fileDialog = new wxFileDialog(this, wxT("Open an image"), wxT(""), wxT(""),
        wxT("BMP image (*.bmp)|*.bmp|JPEG image (*.jpg)|*.jpg|PNG image (*.png)|*.png"), 
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (fileDialog->ShowModal() == wxID_OK)
    {
        wxString fileName = fileDialog->GetPath();
        Settings::BackgroundImage = std::string(fileName.mb_str());
        Settings::IsBackgroundOn = true;

        INVALIDATE();
    }
}

void MainWindow::OnBackgroundView(wxCommandEvent& event)
{
    Settings::IsBackgroundOn = !Settings::IsBackgroundOn;
    INVALIDATE();
}

void MainWindow::OnBackgroundViewUI(wxUpdateUIEvent& event)
{
    event.Check(Settings::IsBackgroundOn);
}

void MainWindow::OnBackgroundStretchRepeat(wxCommandEvent& event)
{
    int id = event.GetId();
    Settings::IsBackgroundStretched = (id == ID_VIEW_BACKGROUND_STRETCH);

    if (Settings::IsBackgroundOn)
    {
        INVALIDATE();
    }
}

void MainWindow::OnBackgroundStretchUI(wxUpdateUIEvent& event)
{
    event.Check(Settings::IsBackgroundStretched);
}

void MainWindow::OnBackgroundRepeatUI(wxUpdateUIEvent& event)
{
    event.Check(!Settings::IsBackgroundStretched);
}

void MainWindow::OnBackgroundInterpolation(wxCommandEvent& event)
{
    int id = event.GetId();
    Settings::BackgroundInterpolation = id - ID_VIEW_BACKGROUND_INTERPOLATION_LINEAR;
    if (Settings::IsBackgroundOn)
    {
        INVALIDATE();
    }
}

void MainWindow::OnBackgroundInterpolationLinearUI(wxUpdateUIEvent& event)
{
    int id = event.GetId();
    event.Check(Settings::BackgroundInterpolation == (id - ID_VIEW_BACKGROUND_INTERPOLATION_LINEAR));
}

void MainWindow::OnBackgroundInterpolationBilinearUI(wxUpdateUIEvent& event)
{
    int id = event.GetId();
    event.Check(Settings::BackgroundInterpolation == (id - ID_VIEW_BACKGROUND_INTERPOLATION_LINEAR));
}

void MainWindow::OnChangeAction(wxCommandEvent& event)
{
    Settings::SelectedAction = event.GetId();
}

void MainWindow::OnSelectTranslateUI(wxUpdateUIEvent& event)
{
    event.Check(Settings::SelectedAction == ID_ACTION_TRANSLATE);
}

void MainWindow::OnSelectScaleUI(wxUpdateUIEvent& event)
{
    event.Check(Settings::SelectedAction == ID_ACTION_SCALE);
}

void MainWindow::OnSelectRotateUI(wxUpdateUIEvent& event)
{
    event.Check(Settings::SelectedAction == ID_ACTION_ROTATE);
}

void MainWindow::OnChangeAxis(wxCommandEvent& event)
{
    int id = event.GetId() - ID_AXIS_X;
    Settings::SelectedAxis[id] = !Settings::SelectedAxis[id];
}

void MainWindow::OnChangeAxisUI(wxUpdateUIEvent& event)
{
    int id = event.GetId() - ID_AXIS_X;
    event.Check(Settings::SelectedAxis[id]);
}

void MainWindow::OnChangeSpace(wxCommandEvent& event)
{
    Settings::SelectedSpace = event.GetId();
}

void MainWindow::OnSelectObjectSpaceUI(wxUpdateUIEvent& event)
{
    event.Check(Settings::SelectedSpace == ID_SPACE_OBJECT);
}

void MainWindow::OnSelectWorldSpaceUI(wxUpdateUIEvent& event)
{
    event.Check(Settings::SelectedSpace == ID_SPACE_WORLD);
}

void MainWindow::OnSelectViewSpaceUI(wxUpdateUIEvent& event)
{
    event.Check(Settings::SelectedSpace == ID_SPACE_VIEW);
}

void MainWindow::OnAnimationSettings(wxCommandEvent& event)
{
    AnimationDialog* dlg = new AnimationDialog("Animation Options");
    dlg->ShowModal();
}

void MainWindow::OnAnimationRecord(wxCommandEvent& event)
{
    if (Scene::GetInstance().GetModels().size() == 0)
        return;
    
    Settings::IsRecording = !Settings::IsRecording;
    LOG_TRACE("MainWindow::OnAnimationRecord: Changed IsRecording to {0}", Settings::IsRecording);
    if (Settings::IsRecording)
    {
        Scene::GetInstance().AddKeyFrame();
    }
}

void MainWindow::OnAnimationRecordUI(wxUpdateUIEvent& event)
{
    event.Check(Settings::IsRecording);
}

void MainWindow::OnAnimationPlay(wxCommandEvent& event)
{
    Settings::IsPlayingAnimation = true;

    do
    {
        clock_t before = clock();
        // m_DrawingPanel->Refresh();
        INVALIDATE();
        clock_t diff = clock() - before;

        double diffSec = (double)diff / CLOCKS_PER_SEC;
        while(diffSec < (1.0 / Settings::FramesPerSeconds))
        {
            diff = clock() - before;
            diffSec = (double)diff / CLOCKS_PER_SEC;
        }
    }
    while (Scene::GetInstance().PlayAnimation());

    Settings::IsPlayingAnimation = false;
    INVALIDATE();
}

void MainWindow::OnAnimationIncreasePlaybackSpeed(wxCommandEvent& event)
{
    Scene::GetInstance().IncreasePlaybackSpeed(25.0);
}

void MainWindow::OnAnimationDecreasePlaybackSpeed(wxCommandEvent& event)
{
    Scene::GetInstance().DecreasePlaybackSpeed(25.0);
}

void MainWindow::OnAnimationNormalPlaybackSpeed(wxCommandEvent& event)
{
    Scene::GetInstance().NormalPlaybackSpeed();
}

/**************************** Private Methods ****************************/

void MainWindow::CreateMenuBar()
{
    // Create MenuBar
    wxMenuBar* menubar = new wxMenuBar();
    
    // Create File Menu
    wxMenu* file = new wxMenu();
    
    // Create Open menu item and connect it
    file->Append(wxID_OPEN, wxT("&Open Model"), wxT("Open a new model file"));
    Connect(wxID_OPEN, wxEVT_COMMAND_MENU_SELECTED, 
        wxCommandEventHandler(MainWindow::OnOpenFile));
    file->AppendSeparator();
    // Create Exit menu item and connect it
    file->Append(wxID_EXIT, wxT("&Exit"));
    Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainWindow::OnQuit));
    // Append the file menu to the menubar
    menubar->Append(file, wxT("&File"));

    // Create View menu
    wxMenu* view = new wxMenu();
    // Create Projection SubMenu
    CreateProjectionSubMenu(view);
    view->AppendCheckItem(ID_VIEW_BACKFACE, wxT("&BackFace Culling"));
    Connect(ID_VIEW_BACKFACE, wxEVT_COMMAND_MENU_SELECTED, 
        wxCommandEventHandler(MainWindow::OnBackFaceCulling));
    // Create Background SubMenu
    CreateBackgroundSubMenu(view);

    // Append the View Menu to the menubar
    menubar->Append(view, wxT("&View"));

    // Create Actions Menu
    wxMenu* actions = new wxMenu();
    actions->AppendCheckItem(ID_ACTION_TRANSLATE, wxT("&Translte"));
    Connect(ID_ACTION_TRANSLATE, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnChangeAction));
    actions->AppendCheckItem(ID_ACTION_SCALE, wxT("&Scale"));
    Connect(ID_ACTION_SCALE, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnChangeAction));
    actions->AppendCheckItem(ID_ACTION_ROTATE, wxT("&Rotate"));
    Connect(ID_ACTION_ROTATE, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnChangeAction));
    //Create Axis submenu
    wxMenu* axis = new wxMenu();
    axis->AppendCheckItem(ID_AXIS_X, wxT("X"));
    Connect(ID_AXIS_X, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnChangeAxis));
    axis->AppendCheckItem(ID_AXIS_Y, wxT("Y"));
    Connect(ID_AXIS_Y, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnChangeAxis));
    axis->AppendCheckItem(ID_AXIS_Z, wxT("Z"));
    Connect(ID_AXIS_Z, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnChangeAxis));
    // Create Space submenu
    wxMenu* space = new wxMenu();
    space->AppendCheckItem(ID_SPACE_OBJECT, wxT("Object"));
    Connect(ID_SPACE_OBJECT, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnChangeSpace));
    space->AppendCheckItem(ID_SPACE_WORLD, wxT("World"));
    Connect(ID_SPACE_WORLD, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnChangeSpace));
    space->AppendCheckItem(ID_SPACE_VIEW, wxT("View"));
    Connect(ID_SPACE_VIEW, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnChangeSpace));
    // Append axis and space submenus to actions menu
    actions->AppendSeparator();
    actions->AppendSubMenu(axis, wxT("&Axis"));
    actions->AppendSeparator();
    actions->AppendSubMenu(space, wxT("&Space"));
    // Append the Actions Menu to the menubar
    menubar->Append(actions, wxT("&Actions"));

    // Create Animation Menu
    wxMenu* animation = new wxMenu();
    animation->Append(ID_ANIMATION_SETTINGS, wxT("&Settings..."));
    Connect(ID_ANIMATION_SETTINGS, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnAnimationSettings));
    animation->AppendCheckItem(ID_ANIMATION_RECORD, wxT("&Record"));
    Connect(ID_ANIMATION_RECORD, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnAnimationRecord));
    animation->AppendSeparator();
    animation->Append(ID_ANIMATION_START_PLAYING, wxT("Start &Playing"));
    Connect(ID_ANIMATION_START_PLAYING, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnAnimationPlay));
    // Create Playback Speed submenu
    wxMenu* speed = new wxMenu();
    speed->Append(ID_ANIMATION_SPEED_INCREASE, wxT("&Increase by 25%"));
    Connect(ID_ANIMATION_SPEED_INCREASE, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnAnimationIncreasePlaybackSpeed));
    speed->Append(ID_ANIMATION_SPEED_DECREASE, wxT("&Decrease by 25%"));
    Connect(ID_ANIMATION_SPEED_DECREASE, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnAnimationDecreasePlaybackSpeed));
    speed->Append(ID_ANIMATION_SPEED_NORMAL, wxT("&Normal"));
    Connect(ID_ANIMATION_SPEED_NORMAL, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnAnimationNormalPlaybackSpeed));
    // Append Playback Speed submenu to actions menu
    animation->AppendSubMenu(speed, wxT("&Playback Speed"));
    // Create Keyframes submenu
    wxMenu* keyframes = new wxMenu();
    keyframes->Append(ID_ANIMATION_KEYFRAMES_FIRST, wxT("&First KeyFrame"));
    keyframes->Append(ID_ANIMATION_KEYFRAMES_LAST, wxT("&Last KeyFrame"));
    keyframes->Append(ID_ANIMATION_KEYFRAMES_NEXT, wxT("&Next KeyFrame"));
    keyframes->Append(ID_ANIMATION_KEYFRAMES_PREV, wxT("&Previous KeyFrame"));
    // Append Animation Submenu to actions menu
    animation->AppendSubMenu(keyframes, wxT("&KeyFrames"));
    // Append the Animation Menu to the menubar
    menubar->Append(animation, wxT("A&nimation"));

    SetMenuBar(menubar);
}

void MainWindow::CreateToolBar()
{
    wxImage::AddHandler(new wxPNGHandler());

    // Create Bitmaps
    wxBitmap exit(wxT("icons/exit.png"), wxBITMAP_TYPE_PNG);
    wxBitmap newb(wxT("icons/new.png"), wxBITMAP_TYPE_PNG);
    wxBitmap open(wxT("icons/open.png"), wxBITMAP_TYPE_PNG);
    wxBitmap ortho(wxT("icons/orthographic.png"), wxBITMAP_TYPE_PNG);
    wxBitmap persp(wxT("icons/perspective.png"), wxBITMAP_TYPE_PNG);
    wxBitmap backface(wxT("icons/back_face.png"), wxBITMAP_TYPE_PNG);
    wxBitmap translate(wxT("icons/translate.png"), wxBITMAP_TYPE_PNG);
    wxBitmap scale(wxT("icons/scale.png"), wxBITMAP_TYPE_PNG);
    wxBitmap rotate(wxT("icons/rotate.png"), wxBITMAP_TYPE_PNG);
    wxBitmap xaxis(wxT("icons/x_axis.png"), wxBITMAP_TYPE_PNG);
    wxBitmap yaxis(wxT("icons/y_axis.png"), wxBITMAP_TYPE_PNG);
    wxBitmap zaxis(wxT("icons/z_axis.png"), wxBITMAP_TYPE_PNG);
    wxBitmap spaceObj(wxT("icons/space_object.png"), wxBITMAP_TYPE_PNG);
    wxBitmap spaceWorld(wxT("icons/space_world.png"), wxBITMAP_TYPE_PNG);
    wxBitmap spaceView(wxT("icons/space_view.png"), wxBITMAP_TYPE_PNG);
    wxBitmap animsettings(wxT("icons/animation_settings.png"), wxBITMAP_TYPE_PNG);
    wxBitmap record(wxT("icons/record.png"), wxBITMAP_TYPE_PNG);
    wxBitmap play(wxT("icons/play.png"), wxBITMAP_TYPE_PNG);

    // Create Toolbar
    wxToolBar* toolbar = new wxToolBar(this, wxID_ANY);
    toolbar->AddTool(wxID_EXIT, wxT("Exit Application"), exit);
    toolbar->AddSeparator();
    toolbar->AddTool(wxID_NEW, wxT("New Scene"), newb);
    toolbar->AddTool(wxID_OPEN, wxT("Open Model"), open);
    toolbar->SetToolLongHelp(wxID_OPEN, wxT("Open a new model file"));
    toolbar->AddSeparator();
    toolbar->AddCheckTool(ID_VIEW_ORTHO, wxT("Switch to Orthographic Projection"), ortho);
    toolbar->AddCheckTool(ID_VIEW_PERSP, wxT("Switch to Perspective Projection"), persp);
    toolbar->AddCheckTool(ID_VIEW_BACKFACE, wxT("Enable/Disable BackFace Culling"), backface);
    toolbar->AddSeparator();
    toolbar->AddCheckTool(ID_ACTION_TRANSLATE, wxT("Translate"), translate);
    toolbar->AddCheckTool(ID_ACTION_SCALE, wxT("Scale"), scale);
    toolbar->AddCheckTool(ID_ACTION_ROTATE, wxT("Rotate"), rotate);
    toolbar->AddSeparator();
    toolbar->AddCheckTool(ID_AXIS_X, wxT("X"), xaxis);
    toolbar->AddCheckTool(ID_AXIS_Y, wxT("Y"), yaxis);
    toolbar->AddCheckTool(ID_AXIS_Z, wxT("Z"), zaxis);
    toolbar->AddSeparator();
    toolbar->AddCheckTool(ID_SPACE_OBJECT, wxT("Object Space"), spaceObj);
    toolbar->AddCheckTool(ID_SPACE_WORLD, wxT("World Space"), spaceWorld);
    toolbar->AddCheckTool(ID_SPACE_VIEW, wxT("View Space"), spaceView);
    toolbar->AddSeparator();
    toolbar->AddTool(ID_ANIMATION_SETTINGS, wxT("Animation Settings..."), animsettings);
    toolbar->AddCheckTool(ID_ANIMATION_RECORD, wxT("Start Recording"), record);
    toolbar->AddTool(ID_ANIMATION_START_PLAYING, wxT("Play Animation"), play);
    
    toolbar->Realize();

    m_MainSizer->Add(toolbar, 0, wxEXPAND);
    m_MainSizer->Layout();
}

void MainWindow::CreateDrawingPanel()
{
    m_DrawingPanel = new DrawPanel(this);

    m_MainSizer->Add(m_DrawingPanel, 1, wxEXPAND | wxALL, 5);
    m_MainSizer->Layout();
}

void MainWindow::CreateProjectionSubMenu(wxMenu* viewMenu)
{
    wxMenu* projection = new wxMenu();
    projection->AppendCheckItem(ID_VIEW_ORTHO, wxT("Orthographic"));
    Connect(ID_VIEW_ORTHO, wxEVT_COMMAND_MENU_SELECTED, 
        wxCommandEventHandler(MainWindow::OnSwitchProjection));
    projection->AppendCheckItem(ID_VIEW_PERSP, wxT("Perspective"));
    Connect(ID_VIEW_PERSP, wxEVT_COMMAND_MENU_SELECTED, 
        wxCommandEventHandler(MainWindow::OnSwitchProjection));
    // Add Projection SubMenu to View Menu
    viewMenu->AppendSubMenu(projection, wxT("&Projection"));
}

void MainWindow::CreateBackgroundSubMenu(wxMenu* viewMenu)
{
    wxMenu* background = new wxMenu();
    background->Append(ID_VIEW_BACKGROUND_OPEN, wxT("&Open..."));
    Connect(ID_VIEW_BACKGROUND_OPEN, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnBackgroundOpen));
    background->AppendCheckItem(ID_VIEW_BACKGROUND_VIEW, wxT("&View"));
    Connect(ID_VIEW_BACKGROUND_VIEW, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnBackgroundView));
    background->AppendSeparator();
    background->AppendCheckItem(ID_VIEW_BACKGROUND_STRETCH, wxT("&Stretch"));
    Connect(ID_VIEW_BACKGROUND_STRETCH, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnBackgroundStretchRepeat));
    background->AppendCheckItem(ID_VIEW_BACKGROUND_REPEAT, wxT("&Repeat"));
    Connect(ID_VIEW_BACKGROUND_REPEAT, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnBackgroundStretchRepeat));
    background->AppendSeparator();
    // Create Interpolation SubMenu
    wxMenu* interpolation = new wxMenu();
    interpolation->AppendCheckItem(ID_VIEW_BACKGROUND_INTERPOLATION_LINEAR, wxT("&Linear"));
    Connect(ID_VIEW_BACKGROUND_INTERPOLATION_LINEAR, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnBackgroundInterpolation));
    interpolation->AppendCheckItem(ID_VIEW_BACKGROUND_INTERPOLATION_BILINEAR, wxT("&Bilinear"));
    Connect(ID_VIEW_BACKGROUND_INTERPOLATION_BILINEAR, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainWindow::OnBackgroundInterpolation));
    // Add Interpolation SubMenu to background
    background->AppendSubMenu(interpolation, wxT("&Interpolation"));
    // Add background SubMenu to view
    viewMenu->AppendSubMenu(background, wxT("&Background"));
}

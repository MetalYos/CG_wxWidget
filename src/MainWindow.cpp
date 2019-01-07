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
    bool isPerspective = Scene::GetInstance().GetCamera()->IsPerpsective();
    event.Check(!isPerspective);
}

void MainWindow::OnSwitchToPerspUI(wxUpdateUIEvent& event)
{
    bool isPerspective = Scene::GetInstance().GetCamera()->IsPerpsective();
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

/**************************** Private Methods ****************************/

void MainWindow::CreateMenuBar()
{
    // Create MenuBar
    wxMenuBar* menubar = new wxMenuBar();
    
    // Create File Menu
    wxMenu* file = new wxMenu();
    
    // Create Open menu item and connect it
    file->Append(wxID_OPEN, wxT("&Open Model"));
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
    wxMenu* projection = new wxMenu();
    projection->AppendCheckItem(ID_VIEW_ORTHO, wxT("Orthographic"));
    Connect(ID_VIEW_ORTHO, wxEVT_COMMAND_MENU_SELECTED, 
        wxCommandEventHandler(MainWindow::OnSwitchProjection));
    projection->AppendCheckItem(ID_VIEW_PERSP, wxT("Perspective"));
    Connect(ID_VIEW_PERSP, wxEVT_COMMAND_MENU_SELECTED, 
        wxCommandEventHandler(MainWindow::OnSwitchProjection));
    // Add Projection SubMenu to View Menu
    view->AppendSubMenu(projection, wxT("&Projection"));
    view->AppendCheckItem(ID_VIEW_BACKFACE, wxT("&BackFace Culling"));
    Connect(ID_VIEW_BACKFACE, wxEVT_COMMAND_MENU_SELECTED, 
        wxCommandEventHandler(MainWindow::OnBackFaceCulling));
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
    animation->AppendCheckItem(ID_ANIMATION_START_RECORDING, wxT("Start &Recording"));
    animation->AppendCheckItem(ID_ANIMATION_STOP_RECORDING, wxT("Stop Recording"));
    animation->AppendSeparator();
    animation->Append(ID_ANIMATION_START_PLAYING, wxT("Start &Playing"));
    // Append the Animation Menu to the menubar
    menubar->Append(animation, wxT("A&nimation"));

    SetMenuBar(menubar);
}

void MainWindow::CreateToolBar()
{
    wxImage::AddHandler(new wxPNGHandler());

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

    wxToolBar* toolbar = new wxToolBar(this, wxID_ANY);
    toolbar->AddTool(wxID_EXIT, wxT("Exit Application"), exit);
    toolbar->AddSeparator();
    toolbar->AddTool(wxID_NEW, wxT("New Scene"), newb);
    toolbar->AddTool(wxID_OPEN, wxT("Open Model"), open);
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
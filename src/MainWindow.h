#pragma once

#include "pch.h"
#include "DrawPanel.h"

class MainWindow : public wxFrame
{
public:
    MainWindow(const wxString& title);

    void OnQuit(wxCommandEvent& event);
    void OnOpenFile(wxCommandEvent& event);
    void OnDrawingPanelPaint(wxPaintEvent& event);
    void OnSwitchProjection(wxCommandEvent& event);
    void OnUpdateUI(wxUpdateUIEvent& event);

    // Action option events
    void OnChangeAction(wxCommandEvent& event);
    void OnSelectTranslateUI(wxUpdateUIEvent& event);
    void OnSelectScaleUI(wxUpdateUIEvent& event);
    void OnSelectRotateUI(wxUpdateUIEvent& event);

    // Axis option events
    void OnChangeAxis(wxCommandEvent& event);
    void OnChangeAxisUI(wxUpdateUIEvent& event);

    // Space option events
    void OnChangeSpace(wxCommandEvent& event);
    void OnSelectObjectSpaceUI(wxUpdateUIEvent& event);
    void OnSelectWorldSpaceUI(wxUpdateUIEvent& event);
    void OnSelectViewSpaceUI(wxUpdateUIEvent& event);

private:
    void InitializeSettings();
    void CreateMenuBar();
    void CreateToolBar();
    void CreateDrawingPanel();

private:
    wxBoxSizer* m_MainSizer;
    wxString m_ModelFileName;
    DrawPanel* m_DrawingPanel = NULL;
};
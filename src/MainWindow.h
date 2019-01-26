#pragma once

#include "pch.h"
#include "DrawPanel.h"

class MainWindow : public wxFrame
{
public:
    MainWindow(const wxString& title);

    void OnDrawingPanelPaint(wxPaintEvent& event);
    void OnUpdateUI(wxUpdateUIEvent& event);

    // File options events
    void OnOpenFile(wxCommandEvent& event);
    void OnClearAll(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);

    // View options events
    void OnSwitchProjection(wxCommandEvent& event);
    void OnSwitchToOrthoUI(wxUpdateUIEvent& event);
    void OnSwitchToPerspUI(wxUpdateUIEvent& event);
    void OnBoundingBox(wxCommandEvent& event);
    void OnBoundingBoxUI(wxUpdateUIEvent& event);
    void OnBackFaceCulling(wxCommandEvent& event);
    void OnBackFaceCullingUI(wxUpdateUIEvent& event);
    void OnBackgroundOpen(wxCommandEvent& event);
    void OnBackgroundView(wxCommandEvent& event);
    void OnBackgroundViewUI(wxUpdateUIEvent& event);
    void OnBackgroundStretchRepeat(wxCommandEvent& event);
    void OnBackgroundStretchUI(wxUpdateUIEvent& event);
    void OnBackgroundRepeatUI(wxUpdateUIEvent& event);
    void OnBackgroundInterpolation(wxCommandEvent& event);
    void OnBackgroundInterpolationLinearUI(wxUpdateUIEvent& event);
    void OnBackgroundInterpolationBilinearUI(wxUpdateUIEvent& event);

    // Action option events
    void OnChangeAction(wxCommandEvent& event);
    void OnSelectSelectUI(wxUpdateUIEvent& event);
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

    // Animation menu events
    void OnAnimationSettings(wxCommandEvent& event);
    void OnAnimationRecord(wxCommandEvent& event);
    void OnAnimationRecordUI(wxUpdateUIEvent& event);
    void OnAnimationPlay(wxCommandEvent& event);

    // Animation Playback Speed menu events
    void OnAnimationIncreasePlaybackSpeed(wxCommandEvent& event);
    void OnAnimationDecreasePlaybackSpeed(wxCommandEvent& event);
    void OnAnimationNormalPlaybackSpeed(wxCommandEvent& event);

    // Rendering menu events
    void OnRenderingSetMaterial(wxCommandEvent& event);

private:
    void CreateMenuBar();
    void CreateToolBar();
    void CreateDrawingPanel();

    wxMenu* CreateFileMenu();

    wxMenu* CreateActionsMenu();
    void CreateAxisSubMenu(wxMenu* actionsMenu);
    void CreateSpacesSubMenu(wxMenu* actionsMenu);

    wxMenu* CreateViewMenu();
    void CreateProjectionSubMenu(wxMenu* viewMenu);
    void CreateBackgroundSubMenu(wxMenu* viewMenu);

    wxMenu* CreateAnimationMenu();
    void CreatePlaybackSpeedSubMenu(wxMenu* animationMenu);
    void CreateKeyFramesSubMenu(wxMenu* animationMenu);

    wxMenu* CreateRenderingMenu();

private:
    wxBoxSizer* m_MainSizer;
    wxString m_ModelFileName;
    wxString m_BGFileName;
    DrawPanel* m_DrawingPanel = NULL;
};
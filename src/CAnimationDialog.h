#pragma once

#include "pch.h"
#include <wx/spinctrl.h>

class AnimationDialog : public wxDialog
{
public:
    AnimationDialog(const wxString& title);

    void OnOkButtonClicked(wxCommandEvent& event);

private:
    wxSpinCtrl* fpsSpinCtrl;
    wxRadioButton* rbLinear;
    wxRadioButton* rbBezier;
};
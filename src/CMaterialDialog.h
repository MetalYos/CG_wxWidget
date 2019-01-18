#pragma once

#include "pch.h"
#include "Material.h"
#include <wx/spinctrl.h>

class MaterialDialog : public wxDialog
{
public:
    MaterialDialog(const wxString& title);
    Material GetMaterial() const;
    void OnColorButtonClicked(wxCommandEvent& event);

private:
    wxSpinCtrl* fpsSpinCtrl;
    wxRadioButton* rbLinear;
    wxRadioButton* rbBezier;

    Material material;
};
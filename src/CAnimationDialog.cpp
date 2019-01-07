#include "CAnimationDialog.h"

AnimationDialog::AnimationDialog(const wxString& title)
    : wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(350, 230))
{
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* text = new wxStaticText(this, wxID_ANY, wxT("Frames Per Second (1 - 100):"));
    fpsSpinCtrl = new wxSpinCtrl(this, IDC_ANIMATION_TEXTCTRL_FPS);
    fpsSpinCtrl->SetRange(1, 100);
    fpsSpinCtrl->SetValue(Settings::FramesPerSeconds);
    hbox1->Add(text, 1, wxALL, 10);
    hbox1->Add(fpsSpinCtrl, 1, wxEXPAND | wxALL, 5);
    vbox->Add(hbox1, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    wxStaticText* text2 = new wxStaticText(this, wxID_ANY, "Interpolation Method:");
    vbox->Add(text2, 1, wxALL, 10);

    wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
    rbLinear = new wxRadioButton(this, IDC_ANIMATION_RADIO_LINEAR, wxT("Linear"));
    rbBezier = new wxRadioButton(this, IDC_ANIMATION_RADIO_BEZIER, wxT("Bezier"));
    hbox2->Add(rbLinear, 1, wxEXPAND | wxALL, 5);
    hbox2->Add(rbBezier, 1, wxEXPAND | wxALL, 5);
    vbox->Add(hbox2, 0, wxALIGN_LEFT | wxTOP | wxBOTTOM, 10);

    wxBoxSizer* hbox3 = new wxBoxSizer(wxHORIZONTAL);
    wxButton* okButton = new wxButton(this, wxID_OK, wxT("Ok"));
    Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, 
        wxCommandEventHandler(AnimationDialog::OnOkButtonClicked));
    wxButton* closeButton = new wxButton(this, wxID_CANCEL, wxT("Cancel"));
    hbox3->Add(okButton, 1);
    hbox3->Add(closeButton, 1, wxLEFT, 5);
    vbox->Add(hbox3, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    SetSizer(vbox);
}

void AnimationDialog::OnOkButtonClicked(wxCommandEvent& event)
{
    Settings::FramesPerSeconds = fpsSpinCtrl->GetValue();
    Settings::FramesInterpolation[0] = rbLinear->GetValue();
    Settings::FramesInterpolation[1] = rbBezier->GetValue();

    Close();
    Destroy();
}
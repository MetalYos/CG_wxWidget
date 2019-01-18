#include "CMaterialDialog.h"

MaterialDialog::MaterialDialog(const wxString& title)
    : wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(350, 350))
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxGridSizer* gridSizer = new wxGridSizer(5, 2, 15, 3);

    gridSizer->Add(new wxStaticText(this, -1, wxT("Color")), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL);
    wxButton* colorBtn = new wxButton(this, IDC_MATERIAL_SELECT_COLOR, wxT("Choose..."));
    Connect(IDC_MATERIAL_SELECT_COLOR, wxEVT_COMMAND_BUTTON_CLICKED, 
        wxCommandEventHandler(MaterialDialog::OnColorButtonClicked));
    gridSizer->Add(colorBtn, 0, wxEXPAND);

    gridSizer->Add(new wxStaticText(this, -1, wxT("Ka")), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL);
    gridSizer->Add(new wxButton(this, -1, wxT("Choose...")), 0, wxEXPAND);
    gridSizer->Add(new wxStaticText(this, -1, wxT("Kd")), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL);
    gridSizer->Add(new wxButton(this, -1, wxT("Choose...")), 0, wxEXPAND);
    gridSizer->Add(new wxStaticText(this, -1, wxT("Ks")), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL);
    gridSizer->Add(new wxButton(this, -1, wxT("Choose...")), 0, wxEXPAND);
    gridSizer->Add(new wxStaticText(this, -1, wxT("Specular")), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL);
    gridSizer->Add(new wxTextCtrl(this, -1, wxT("32")), 0, wxEXPAND);
    mainSizer->Add(gridSizer, 1, wxEXPAND | wxALL, 10);

    wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
    wxButton *btn1 = new wxButton(this, wxID_OK, wxT("Ok"));
    hbox->Add(btn1, 0);
    wxButton *btn2 = new wxButton(this, wxID_CANCEL, wxT("Cancel"));
    hbox->Add(btn2, 0, wxLEFT | wxBOTTOM , 5);
    mainSizer->Add(hbox, 0, wxALIGN_RIGHT | wxRIGHT, 10);

    SetSizer(mainSizer);

    Centre();
}

void MaterialDialog::OnColorButtonClicked(wxCommandEvent& event)
{
    wxColourDialog dlg(this);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxColourData data = dlg.GetColourData();
        wxColour color = data.GetColour();
        material.Color[0] = color.Red();
        material.Color[1] = color.Green();
        material.Color[2] = color.Blue();
    }
    dlg.Close();
    dlg.Destroy();
}

Material MaterialDialog::GetMaterial() const
{
    return material;
}

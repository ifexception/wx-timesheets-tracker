//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2018> <Szymon Welgus>
//
//  This program is free software : you can redistribute it and /
//  or modify it under the terms of the GNU General Public License as published
//  by the Free Software Foundation
//  , either version 3 of the License
//  , or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful
//  , but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "category_dialog.h"

#include <vector>
#include <wx/statline.h>

#include "../common/common.h"
#include "../common/ids.h"
#include "../services/db_service.h"

namespace app::dialog
{
wxIMPLEMENT_DYNAMIC_CLASS(category_dialog, wxDialog);

wxBEGIN_EVENT_TABLE(category_dialog, wxDialog)
    EVT_BUTTON(ids::ID_SAVE, category_dialog::on_save)
    EVT_BUTTON(wxID_CANCEL, category_dialog::on_cancel)
wxEND_EVENT_TABLE()

category_dialog::category_dialog(wxWindow* parent, bool isEdit, const wxString& name)
    : mProjectChoiceId(-1)
    , mNameText(wxGetEmptyString())
    , mDescriptionText(wxGetEmptyString())
{
    wxString title;
    if (isEdit) {
        title = wxT("Edit Category");
    } else {
        title = wxT("Add Category");
    }

    bool success = create(parent, wxID_ANY, title, wxDefaultPosition, wxSize(320, 240), wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU, name);
}

category_dialog::~category_dialog()
{
    Destroy();
}

void category_dialog::launch_dialog()
{
    ShowModal();
}

bool category_dialog::create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& position, const wxSize& size, long style, const wxString& name)
{
    bool created = wxDialog::Create(parent, windowId, title, position, size, style, name);
    if (created) {
        create_controls();
        fill_post_create();

        GetSizer()->Fit(this);
        //SetIcon();
        Centre();
    }

    return false;
}

void category_dialog::create_controls()
{
    /* Window Sizing */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    auto mainPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(mainPanelSizer, common::sizers::ControlDefault);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    mainPanelSizer->Add(sizer, 0);

    /* Task Details Box */
    auto detailsBox = new wxStaticBox(this, wxID_ANY, wxT("Category Details"));
    auto detailsBoxSizer = new wxStaticBoxSizer(detailsBox, wxVERTICAL);
    sizer->Add(detailsBoxSizer, common::sizers::ControlExpandProp);

    auto categoryDetailsPanel = new wxPanel(this, wxID_STATIC);
    detailsBoxSizer->Add(categoryDetailsPanel, common::sizers::ControlExpand);

    auto flexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    categoryDetailsPanel->SetSizer(flexGridSizer);

    /* ---Controls--- */
    /* Project Choice Control */
    auto projectChoiceText = new wxStaticText(categoryDetailsPanel, wxID_STATIC, wxT("Project"));
    flexGridSizer->Add(projectChoiceText, common::sizers::ControlCenterVertical);

    pProjectChoiceCtrl = new wxChoice(categoryDetailsPanel, IDC_PROJECTCHOICE, wxDefaultPosition, wxSize(150, -1));
    pProjectChoiceCtrl->AppendString(wxT("Select a project"));
    pProjectChoiceCtrl->SetSelection(0);
    pProjectChoiceCtrl->SetToolTip(wxT("Select a project to associate this category with"));
    flexGridSizer->Add(pProjectChoiceCtrl, common::sizers::ControlDefault);

    /* Category Name Text Control */
    auto nameText = new wxStaticText(categoryDetailsPanel, wxID_STATIC, wxT("Name"));
    flexGridSizer->Add(nameText, common::sizers::ControlCenterVertical);

    pNameCtrl = new wxTextCtrl(categoryDetailsPanel, IDC_NAME, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT, wxDefaultValidator, wxT("category_name_ctrl"));
    pNameCtrl->SetToolTip(wxT("Enter a name for this category"));
    flexGridSizer->Add(pNameCtrl, common::sizers::ControlDefault);

    /* Category Description Text Control */
    auto descriptionText = new wxStaticText(categoryDetailsPanel, wxID_STATIC, wxT("Description"));
    flexGridSizer->Add(descriptionText, common::sizers::ControlCenterVertical);

    pDescriptionCtrl = new wxTextCtrl(this, IDC_DESCRIPTION, wxGetEmptyString(), wxDefaultPosition, wxSize(240, 160), wxTE_MULTILINE, wxDefaultValidator, wxT("category_description_ctrl"));
    pDescriptionCtrl->SetToolTip(wxT("Enter a detailed description of a task category"));
    detailsBoxSizer->Add(pDescriptionCtrl, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    /* Horizontal Line*/
    auto separation_line = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL, wxT("category_static_line"));
    mainSizer->Add(separation_line, 0, wxEXPAND | wxALL, 1);

    /* Button Panel */
    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainSizer->Add(buttonPanel, wxSizerFlags(wxSizerFlags().Border(wxALL, 5)).Center());

    auto okButton = new wxButton(buttonPanel, ids::ID_SAVE, wxT("&Save"));
    auto cancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("&Cancel"));

    buttonPanelSizer->Add(okButton, wxSizerFlags().Border(wxALL, 5));
    buttonPanelSizer->Add(cancelButton, wxSizerFlags().Border(wxALL, 5));
}

void category_dialog::fill_post_create()
{
    std::vector<models::project> projects;
    try {
        services::db_service dbService;
        projects = dbService.get_projects();
    } catch (const std::exception& e) {
        wxLogDebug(e.what());
    }

    for (auto p : projects) {
        pProjectChoiceCtrl->Append(p.display_name, (void*)p.project_id);
    }
}

bool category_dialog::validate()
{
    if (mProjectChoiceId == -1) {
        wxMessageBox(wxT("Project is required"), wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    if (mNameText.length() > 255 || mNameText.length() < 2 || mNameText.empty()) {
        wxMessageBox(wxT("Category name is invalid"), wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    if (mDescriptionText.length() > 512 || mDescriptionText.length() < 8 || mDescriptionText.empty()) {
        wxMessageBox(wxT("Category description is invalid"), wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }
    return true;
}

bool category_dialog::are_controls_empty()
{
    bool isEmpty = mProjectChoiceId == -1 && mNameText.empty() && mDescriptionText.empty();
    return isEmpty;
}

void category_dialog::on_save(wxCommandEvent& event)
{
    mProjectChoiceId = (int)pProjectChoiceCtrl->GetClientData(pProjectChoiceCtrl->GetSelection());
    mNameText = pNameCtrl->GetValue();
    mDescriptionText = pDescriptionCtrl->GetValue();

    bool isValid = validate();
    if (!isValid) {
        return;
    }

    try {
        services::db_service dbService;
        dbService.create_new_category(mProjectChoiceId, std::string(mNameText.ToUTF8()), std::string(mDescriptionText.ToUTF8()));
    } catch (const std::exception& e) {
        wxLogDebug(e.what());
    }

    EndModal(ids::ID_SAVE);
}

void category_dialog::on_cancel(wxCommandEvent& event)
{
    bool areControlsEmpty = are_controls_empty();
    if (!areControlsEmpty) {

        int answer = wxMessageBox(wxT("Are you sure you want to exit?"), wxT("Confirm"),
            wxYES_NO | wxICON_QUESTION);
        if (answer == wxYES) {
            EndModal(wxID_CANCEL);
        }
    } else {
        EndModal(wxID_CANCEL);
    }
}

} // app::dialog

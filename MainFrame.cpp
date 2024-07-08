#include "MainFrame.h"
#include <wx/wx.h>
#include "FindSave.h"
#include "Constants.h"





MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title)
{
	// populate vector with path data
	appDataPath = finder.GetAppDataPath();
	finder.GetCompanyPaths(appDataPath);
	finder.GetUnusedSavePaths(appDataPath);
	finder.GetUnknownSavePaths(appDataPath);

	wxPanel* panel = new wxPanel(this);
	AddSavePathForm(panel,CONSTANT::UNLINKED_FORM_TITLE);
	AddSavePathForm(panel, CONSTANT::UNKNOWN_FORM_TITLE, 1, CONSTANT::FORM_X_OFFSET);
	CreateStatusBar();







}


void MainFrame::AddSavePathForm(wxPanel* panel,std::string formTitle,int pathType, int posXOffset, int posYOffset)
{
	// buttons
	wxButton* rescanButton = new wxButton(panel,
		wxID_ANY,
		"Rescan",
		wxPoint(CONSTANT::RESCAN_BUTTON_POS.first + posXOffset, CONSTANT::RESCAN_BUTTON_POS.second + posYOffset),
		wxSize(CONSTANT::RESCAN_BUTTON_SIZE.first, CONSTANT::RESCAN_BUTTON_SIZE.second));

	wxButton* deleteButton = new wxButton(panel,
		wxID_ANY,
		"Delete",
		wxPoint(CONSTANT::DELETE_BUTTON_POS.first + posXOffset, CONSTANT::DELETE_BUTTON_POS.second + posYOffset),
		wxSize(CONSTANT::DELETE_BUTTON_SIZE.first, CONSTANT::DELETE_BUTTON_SIZE.second));

	// list title text
	wxStaticText* listTitle = new wxStaticText(panel,
		wxID_ANY,
		formTitle,
		wxPoint(CONSTANT::LIST_TITLE_POS.first + posXOffset, CONSTANT::LIST_TITLE_POS.second + posYOffset));

	// convert string paths to a wxArrayString

	
	wxArrayString choices;

	switch (pathType)
	{
	case 0:
	{
		// unlinked games
		for (const std::string& path : finder.GetUnlinkedPathsVector())
		{
			// ensure it is encoded properly to UT8 if there are symbols.
			std::string gameName = finder.ExtractGameName(path);
			wxString str = wxString::FromUTF8(gameName.c_str());
			choices.Add(str);
		}
		break;
	}
	case 1:
		// unknown games
		for (const std::string& path : finder.GetUnknownPathsVector())
		{
			// ensure it is encoded properly to UT8 if there are symbols.
			std::string gameName = finder.ExtractGameName(path);
			wxString str = wxString::FromUTF8(gameName.c_str());
			choices.Add(str);
		}
		break;

	}


	// checklist

	// populate list with games
	wxCheckListBox* pathList = new wxCheckListBox(panel,
		wxID_ANY,
		wxPoint(CONSTANT::LISTBOX_POS.first + posXOffset, CONSTANT::LISTBOX_POS.second + posYOffset),
		wxSize(CONSTANT::LISTBOX_SIZE.first, CONSTANT::LISTBOX_SIZE.second),
		choices);


	// bind buttons to correct funcs
	switch (pathType)
	{
	case 0:
	{
		rescanButton->Bind(wxEVT_BUTTON, &MainFrame::OnRescanClicked, this);
		deleteButton->Bind(wxEVT_BUTTON, &MainFrame::OnDeleteClicked, this);
	}
	case 1:
		rescanButton->Bind(wxEVT_BUTTON, &MainFrame::OnRescanClicked, this);
		deleteButton->Bind(wxEVT_BUTTON, &MainFrame::OnDeleteClicked, this);

	}

}


void MainFrame::OnDeleteClicked(wxCommandEvent& event)
{
	wxLogStatus("Delete clicked");

}
void MainFrame::OnRescanClicked(wxCommandEvent& event)
{
	wxLogStatus("Rescan clicked");

}
#pragma once
#include <wx/wx.h>
#include "FindSave.h"
#include <string>

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);
	void OnDeleteClicked(wxCommandEvent& event, wxCheckListBox* list, int pathType);
	void OnRescanClicked(wxCommandEvent& event, wxCheckListBox* list, int pathType);
	void RescanDirectory(wxCheckListBox* list, int pathType);
	wxArrayString GenerateCheckListElements(int pathType);
	void AddSavePathForm(wxPanel* wxPanel, std::string formTitle, int pathType = 0, int posXOffset = 0, int posYOffset = 0);

private:
	FindSave finder;
	std::string appDataPath;

};


#pragma once
#include <wx/wx.h>
#include "FindSave.h"
#include <string>

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);
	void AddSavePathForm(wxPanel* wxPanel, std::string formTitle, int pathType = 0, int posXOffset = 0, int posYOffset = 0);

private:
	FindSave finder;
	std::string appDataPath;
	std::vector<std::string> pathList;

};


#include "MainFrame.h"
#include <wx/wx.h>
#include "FindSave.h"
#include "Constants.h"
#include <filesystem>



/**
 * @brief Initialize GUI
 *
 * Initializes all the elements using wxWidgets which include:
 * 2 uneditable text labels
 * 2 CheckListBox
 * 4 buttons
 *
 *
 * @param title The title of the program which appears on top of the program.
 * @return Constructor
 */
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

}

/**
 * @brief Attempts to store all paths to save folders that has no game in system.
 *
 * Goes through LocalAppData, extracts game path from Player.log, and populates the
 * string vector full of paths of save folders with no games in system.
 *
 * @param path The LocalAppData path, gained from the GetAppDataPath function.
 * @return A string vector, full of paths of save folders with no games (if it exists)
 */
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
	wxArrayString choices = GenerateCheckListElements(pathType);
	

	// checklist

	// populate list with games
	wxCheckListBox* pathList = new wxCheckListBox(panel,
		wxID_ANY,
		wxPoint(CONSTANT::LISTBOX_POS.first + posXOffset, CONSTANT::LISTBOX_POS.second + posYOffset),
		wxSize(CONSTANT::LISTBOX_SIZE.first, CONSTANT::LISTBOX_SIZE.second),
		choices);

	// bind buttons to functions
	rescanButton->Bind(wxEVT_BUTTON, [this, pathList, pathType](wxCommandEvent& event) { this->OnRescanClicked(event, pathList, pathType); });
	deleteButton->Bind(wxEVT_BUTTON, [this, pathList, pathType](wxCommandEvent& event) { this->OnDeleteClicked(event, pathList, pathType); });


}

/**
 * @brief Deletes selected elements from the CheckListBox
 *
 * Deletes the save files located in LocalLow and PlayerPrefs in registry.
 * Also checks if the company folder is empty afterwards, and removes that 
 * in LocalLow and Registry.


 * @param event Required for event handling
 * @param list The list of elements in the CheckListBox
 * @param pathType 0 means list with unlinked game paths, whilst 1 is unknown game paths
 *		  used to differentiate which button is pressed and which list to use.
 */
void MainFrame::OnDeleteClicked(wxCommandEvent& event, wxCheckListBox* list, int pathType)
{
	// early return if 0 checked items
	wxArrayInt checkedItems;
	list->GetCheckedItems(checkedItems);
	if (checkedItems.empty())
	{
		wxMessageBox("No checked items");
		return;
	}

	// go thru all list elements, get checked items & delete
	for (int i = 0; i < list->GetCount(); ++i)
	{
		if (list->IsChecked(i))
		{
			std::filesystem::path pathToUTF8;
			// unlinked
			if (pathType == 0)
			{
				// convert to UTF8 incase the path isn't English and uses symbols.
				pathToUTF8 = std::filesystem::u8path(finder.GetUnlinkedPathsVector()[i]);

			}
			// unknown
			else if (pathType == 1)
			{
				// convert to UTF8 incase the path isn't English and uses symbols.
				pathToUTF8 = std::filesystem::u8path(finder.GetUnknownPathsVector()[i]);


			}

			// delete folder
			if (std::filesystem::exists(pathToUTF8))
			{
				try
				{
					std::filesystem::remove_all(pathToUTF8);
				}
				catch (const std::filesystem::filesystem_error& e)
				{
					wxMessageBox("Error deleting file: ", e.what());
				}
				catch (const std::exception& e)
				{
					wxMessageBox("Unexpected error:", e.what());

				}


			}

			// delete PlayerPref key for associated game in the registry.
			finder.DeletePlayerPrefPath(pathToUTF8.u8string());


		}
	}
	finder.RemoveEmptyFolders();




	wxMessageBox("Saves deleted");

	RescanDirectory(list, pathType);
	

}
/**
 * @brief Rescans directory and updates the CheckListBox
 *
 * Clears the CheckListBox, rescans the directory and appends the new list.
 * 
 * 
 * @param event Required for event handling
 * @param list The current list of elements in the CheckListBox
 * @param pathType 0 means list with unlinked game paths, whilst 1 is unknown game paths
 *		  used to differentiate which button is pressed and which list to use.
 */
void MainFrame::OnRescanClicked(wxCommandEvent& event, wxCheckListBox* list, int pathType)
{
	RescanDirectory(list, pathType);

}
/**
 * @brief Rescans directory and updates the CheckListBox
 *
 * Clears the CheckListBox, rescans the directory and appends the new list.
 *
 *
 * @param event Required for event handling
 * @param list The current list of elements in the CheckListBox
 * @param pathType 0 means list with unlinked game paths, whilst 1 is unknown game paths
 *		  used to differentiate which button is pressed and which list to use.
 */
void MainFrame::RescanDirectory(wxCheckListBox* list, int pathType)
{
	// clear list and rescan directory
	list->Clear();
	finder.GetCompanyPaths(appDataPath);
	// update path vector
	switch (pathType)
	{
	case 0:
		finder.GetUnusedSavePaths(appDataPath);
		break;
	case 1:
		finder.GetUnknownSavePaths(appDataPath);
		break;
	}

	// readd new paths
	wxArrayString choices = GenerateCheckListElements(pathType);

	for (const auto& updatedPath : choices)
	{
		list->Append(updatedPath);
	}
}

/**
 * @brief Formats the game path string into a compatible wxString to add to wxArrayString
 *
 * Gets the game path, extracts game name and adds to wxArrayString.
 * Used to rescan directory and update the list.
 *
 *
 * @param pathType 0 means list with unlinked game paths, whilst 1 is unknown game paths
 *		  used to differentiate which button is pressed and which list to use.
 * @returns A list of wxArrayStrings of unlinked/unknown game paths for wxCheckListBox.
 */
wxArrayString MainFrame::GenerateCheckListElements(int pathType)
{
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

	return choices;
}

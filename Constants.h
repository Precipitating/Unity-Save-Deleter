#pragma once
#include <string>
#include <utility>


namespace CONSTANT
{
	const std::string APP_NAME = "Unity Save Deleter";
	const std::pair<int, int> RESOLUTION = std::make_pair(700, 500);
	
	const std::pair<int, int> RESCAN_BUTTON_POS = std::make_pair(40, 455);
	const std::pair<int, int> RESCAN_BUTTON_SIZE = std::make_pair(100, 35);

	const std::pair<int, int> DELETE_BUTTON_POS = std::make_pair(210, 455);
	const std::pair<int, int> DELETE_BUTTON_SIZE = std::make_pair(100, 35);
	
	const std::pair<int, int> LISTBOX_POS = std::make_pair(26, 50);
	const std::pair<int, int> LISTBOX_SIZE = std::make_pair(300, 400);
	
	const std::pair<int, int> LIST_TITLE_POS = std::make_pair(50, 25);

	const std::string UNLINKED_FORM_TITLE = "Unity saves with no game in system:";
	const std::string UNKNOWN_FORM_TITLE = "Unity saves with ??? game in system:";

	const int FORM_X_OFFSET = 350;
}
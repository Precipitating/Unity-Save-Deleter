#include "FindSave.h"
#include <filesystem>
#include <vector>
#include <string>
#include <shlobj.h>
#include <iostream>
#include <fstream>
#include <limits>



/**
 * @brief Attempts to store all paths to save folders that has no game in system.
 *
 * Goes through LocalAppData, extracts game path from Player.log, and populates the 
 * string vector full of paths of save folders with no games in system.
 * 
 *
 *
 * @param path The LocalAppData path, gained from the GetAppDataPath function.
 * @return A string vector, full of paths of save folders with no games (if it exists)
 */
std::vector<std::string> FindSave::GetFolderContents(const std::string& path)
{
	std::vector<std::string> dir;
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			for (const auto& entry2 : std::filesystem::recursive_directory_iterator(entry))
			{
				// find Unity related files, and check if the game exists in system.
				if (entry2.is_regular_file() && entry2.path().filename() == "Player.log")
				{
					std::string path = entry2.path().u8string();
					std::string gameFolder = path.substr(0, path.find_last_of('\\'));
					if (!GameExists(path))
					{
						dir.push_back(gameFolder);
					}

				}
			}
			
		}
	}
	return dir;

}
/**
 * @brief Gets the LocalAppData path, which is one area Unity stores their saves.
 *
 * Simply gives the path to LocalAppData, I don't know the user's desktop name hence why a
 * fixed path isn't an option.
 * 
 * @return The file path to LocalAppData if success, else 'Failed' 
 */
std::string FindSave::GetAppDataPath()
{
	PWSTR path = NULL;
	// get locallow folder
	HRESULT result = SHGetKnownFolderPath(FOLDERID_LocalAppDataLow, 0, NULL, &path);
	if (SUCCEEDED(result))
	{
		std::wstring wpath(path);
		CoTaskMemFree(path); // Free allocated memory
		return std::string(wpath.begin(), wpath.end());
	}
	else
	{
		return "Fail";
	}


}

/**
 * @brief Searches inside Player.log and attempts to find the game path directory.
 *
 * There are a couple of variations on how Player.log is formatted, I identify and
 * extract the game file path for some of these variations.  
 *
 * Then, it checks if the path exists, and if not, it returns true as the game exists
 * in system.
 * 
 * @param path The direct path to the Player.log file, which most Unity games have.
 * for opening and extracting the game file path.
 * @return True if game exists on system, else False.
 */
bool FindSave::GameExists(const std::string& path)
{
	bool result = false;

	std::string stream;
	std::ifstream file;
	file.open(path);
	if (std::getline(file, stream))
	{
		std::string firstLine = stream;
		// First format: Mono path[0] = (game path)
		if (firstLine.find("Mono") != std::string::npos)
		{
			firstLine = firstLine.substr(firstLine.find_first_of("'"), firstLine.find_last_of("'"));
		}
		// Second line format: [Subsystems] Discovering subsystems at path (game path)
		else if (firstLine.find("Loading") != std::string::npos)
		{
			firstLine = firstLine.substr(firstLine.find_first_of(':') - 1, firstLine.size() - 1);

		}
		else
		{
			// go through line by line until we find the line that includes the word 'path',
			// which hopefully guarantees the game path.
			while (stream.find("path") == std::string::npos)
			{
				std::getline(file, stream);
			}
			firstLine = stream;
			firstLine = firstLine.substr(firstLine.find("path") + 5, firstLine.size() - 1);

		}

		// remove apostrophe from string
		firstLine = firstLine.substr(1, firstLine.size() - 2);

		// check if extracted string exists, if it does return true.
		if (std::filesystem::exists(firstLine))
		{
			result = true;
		}

	}

	file.close();
	return result;
}

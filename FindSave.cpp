#include "FindSave.h"
#include <filesystem>
#include <vector>
#include <string>
#include <shlobj.h>
#include <iostream>
#include <fstream>
#include <limits>


/**
 * @brief Store all Unity save folder paths that point to the company name.
 *
 * The intention is to see if the folder is empty after save games have been deleted,
 * as lingering empty folders are useless. It will be deleted if so.
 * 
 *
 * @param path The LocalAppData path, gained from the GetAppDataPath function.
 * @return A string vector, full of paths that point to Unity company names w/ game titles in it.
 */
void FindSave::GetCompanyPaths(const std::string& path)
{
	if (!companyPath.empty())
	{
		companyPath.clear();
	}
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			for (const auto& entry2 : std::filesystem::recursive_directory_iterator(entry))
			{
				// Store all Unity related save file company folder paths.
				// If so, store path and break, as we've already found out its a Unity save folder.
				if (entry2.is_regular_file() && (entry2.path().filename() == "output_log.txt" || entry2.path().filename() == "Player.log"))
				{
					companyPath.push_back(entry.path().u8string());
					break;

				}
			}

		}
	}

}
/**
 * @brief Gets other Unity save paths that don't have 'Player.log'
 *
 * The other standard Unity extra files that exists in save folders from my observation is output_log.txt,
 * which does not specify the game path, so there is no way to check if it exists.
 *
 *
 *
 * @param path The LocalAppData path, gained from the GetAppDataPath function.
 * @return A string vector, full of paths of save folders with output_log.txt only
 */
void FindSave::GetUnknownSavePaths(const std::string& path)
{
	if (!unknownSavePaths.empty())
	{
		unknownSavePaths.clear();
	}
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			for (const auto& entry2 : std::filesystem::recursive_directory_iterator(entry))
			{
				// find Unity related files, and check if the game exists in system.
				if (entry2.is_regular_file() && entry2.path().filename() == "output_log.txt")
				{
					std::string path = entry2.path().u8string();
					// remove Player.log from path to make game name extraction easier
					std::string gameFolder = path.substr(0, path.find_last_of('\\'));

					unknownSavePaths.push_back(gameFolder);



				}
			}

		}
	}

}
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
void FindSave::GetUnusedSavePaths(const std::string& path)
{
	if (!unlinkedSavePaths.empty())
	{
		unlinkedSavePaths.clear();
	}
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
					// remove output_log.txt from path to make game name extraction easier
					std::string gameFolder = path.substr(0, path.find_last_of('\\'));
					if (!GameExists(path))
					{
						unlinkedSavePaths.push_back(gameFolder);
					}


				}
			}
			
		}
	}

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
 * Then, it checks if the path exists, and if so, it returns true as the game exists
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

			// remove apostrophe from string
			firstLine = firstLine.substr(1, firstLine.size() - 2);
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
			firstLine = firstLine.substr(firstLine.find("path") + 5, firstLine.size());
			firstLine = firstLine.substr(0, firstLine.find_last_of('/'));


		}

		std::filesystem::path toUTF8 = std::filesystem::u8path(firstLine);
		// check if extracted string exists, if it does return true.
		if (std::filesystem::exists(toUTF8))
		{
			result = true;
		}

	}

	file.close();
	return result;
}



/**
 * @brief Extracts game name from path.
 *
 * Gets deepest folder in path, getting the last \ to achieve this.
 *
 *
 * @param path Should be a string with the game name as the last folder in path.
 * @returns Game name string
 */
std::string FindSave::ExtractGameName(const std::string& path)
{
	return path.substr(path.find_last_of('\\') + 1, path.size() - 1);
}

/**
 * @brief Remove all empty Unity save folders
 *
 * Should be called after deleting saves, to remove all empty directories.
 *
 */
void FindSave::RemoveEmptyFolders()
{
	for (const std::string& path : companyPath)
	{
		std::filesystem::path pathToUTF8 = std::filesystem::u8path(path);

		if (std::filesystem::is_empty(pathToUTF8))
		{
			std::filesystem::remove(pathToUTF8);
		}
	}
}
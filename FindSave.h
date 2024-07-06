#pragma once
#include <string>
#include <vector>

class FindSave
{
public:
	std::vector<std::string> GetFolderContents(const std::string& path);
	std::string GetAppDataPath();
	bool GameExists(const std::string& path);



private:
	std::string appDataPath;

};


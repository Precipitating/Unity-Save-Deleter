#pragma once
#include <string>
#include <vector>

class FindSave
{
public:
	void GetUnusedSavePaths(const std::string& path);
	void GetCompanyPaths(const std::string& path);
	void GetUnknownSavePaths(const std::string& path);

	std::string GetAppDataPath();
	std::string ExtractGameName(const std::string& path);

	void RemoveEmptyFolders();
	void DeletePlayerPrefPath(const std::string& path);



	std::vector<std::string> GetUnlinkedPathsVector() const { return unlinkedSavePaths; }
	std::vector<std::string> GetUnknownPathsVector() const { return unknownSavePaths; }
	std::vector<std::string> GetCompanyPathsVector() const { return companyPath; }
	



private:
	std::string FwdSlashToBackSlash(const std::string& str);
	bool GameExists(const std::string& path);

	void DeleteEmptyRegistryFolder(const std::wstring& path);

	std::vector<std::string> unlinkedSavePaths;
	std::vector<std::string> unknownSavePaths;
	std::vector<std::string> companyPath;
	std::string appDataPath;

};



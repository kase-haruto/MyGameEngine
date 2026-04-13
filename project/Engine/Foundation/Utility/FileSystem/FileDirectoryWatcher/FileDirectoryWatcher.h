#pragma once
#include <string>

/*-----------------------------------------------------------------------------------------
 * FileDirectoryWatche
 * - 指定したディレクトリを走破してファイルを検索するクラス
 *---------------------------------------------------------------------------------------*/
class FileDirectoryWatcher {
	//===================================================================*/
	//                    public methods
	//===================================================================*/
public:
	FileDirectoryWatcher();
	~FileDirectoryWatcher();

	/**
	 * @brief 指定したディレクトリを走破してファイルを検索する
	*/
	void SearchFiles(const std::string& directoryPath, const std::string& fileExtension);
};

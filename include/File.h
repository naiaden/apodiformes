/*
 * File.h
 *
 *  Created on: Oct 30, 2013
 *      Author: lonrust
 */

#ifndef FILE_H_
#define FILE_H_

#include <string>

class File
{

public:

	File(const std::string& fileName, const std::string& extension, const std::string& directory = "");

        bool exists();
	std::string getFileName(bool withExtension = true) const;
	std::string getName() const;
	std::string getExtension() const;
	std::string getDirectory(bool delim = false) const;
	std::string getPath(bool withExtension = true) const;

	virtual ~File();

private:
	std::string extension;
	std::string fileName;
	std::string directory;
};

class TestFile: public File
{
public:
	TestFile(const std::string& fileName, const std::string& extension, const std::string& directory = "");
};

class TrainFile: public File
{
public:
	TrainFile(const std::string& fileName, const std::string& extension, const std::string& directory = "");
};

class ColibriFile: public File
{

public:
	enum Type
	{
		PATTERNMODEL, ENCODED, CORPUS, UNKNOWN
	};

	ColibriFile(const std::string& fileName, const std::string& extension, const std::string& directory = "", Type type =
	        Type::UNKNOWN);

private:
	Type type;
};

#endif /* FILE_H_ */

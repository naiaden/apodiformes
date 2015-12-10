/*
 * File.cpp
 *
 *  Created on: Oct 30, 2013
 *      Author: lonrust
 */

#include "File.h"
#include <fstream>
//#include <experimental/filesystem>

File::~File()
{
	// TODO Auto-generated destructor stub
}

File::File(const std::string& fileName, const std::string& extension, const std::string& directory)
		: fileName(fileName), extension(extension), directory(directory)
{

}

bool File::exists()
{
    // only in gcc 5+, currently stuck on 4.9-something
    //return fs::exists(getPath(true));
    std::ifstream infile(getPath(true));
    return infile.good();
}

std::string File::getFileName(bool withExtension) const
{
	if (withExtension)
		return fileName + "." + extension;
	return fileName;
}

std::string File::getExtension() const
{
	return extension;
}

std::string File::getDirectory(bool delim) const
{
	if (!delim)
		return directory.substr(0, directory.size()-1);;
	return directory;
}
std::string File::getPath(bool withExtension) const
{
	return getDirectory(true) + getFileName(withExtension);

}

TestFile::TestFile(const std::string& fileName, const std::string& extension, const std::string& directory)
		: File(fileName, extension, directory)
{

}

TrainFile::TrainFile(const std::string& fileName, const std::string& extension, const std::string& directory)
		: File(fileName, extension, directory)
{

}

ColibriFile::ColibriFile(const std::string& fileName, const std::string& extension, const std::string& directory,
        ColibriFile::Type type)
		: File(fileName, extension, directory), type(type)
{

}

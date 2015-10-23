/*
 * Document.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: lonrust
 */

#include "Document.h"

#include <boost/foreach.hpp>

typedef std::unordered_map<const Pattern, double> umap;

int Document::getDocumentId()
{
	return documentId;
}

std::string Document::getFileName()
{
	return fileName;
}

Document::Document(int documentId, const std::string& fileName, ClassDecoder* classDe1coder)
{
	// TODO Auto-generated constructor stub
	this->documentId = documentId;
	this->fileName = fileName;
	this->classDecoder = classDe1coder;
}

Document::~Document()
{
	// TODO Auto-generated destructor stub
}

/**
 * returns 0 if the pattern does not exist in the document
 */
double Document::getValue(const Pattern& pattern) const
{
	std::unordered_map<const Pattern, double>::const_iterator iter = data.find(pattern);

	if (iter != data.end())
	{

		return iter->second;
	}

	return 0;
}

/**
 * If pattern exists in the document, double returns the old value of pattern
 * otherwise the pattern is added with newValue, and 0 is returned.
 */
double Document::updateValue(const Pattern& pattern, double newValue)
{
	std::unordered_map<const Pattern, double>::const_iterator iter = data.find(pattern);

	if (iter != data.end())
	{
		double oldValue = iter->second;
		data[pattern] = newValue;

		return oldValue;
	}

	data[pattern] = newValue;
	return 0;
}

void Document::printPatterns()
{
	std::cout << "(" << documentId << " --- " << fileName << ")" << std::endl;
	BOOST_FOREACH( umap::value_type v, data ){
	Pattern p = v.first;
	std::cout << "[" << p.tostring(*classDecoder) << "]" << v.second << std::endl;
}
}

ClassDecoder* Document::getClassDecoder()
{
	return classDecoder;
}

std::string Document::toString(const Pattern& pattern) const
{
	std::stringstream ss;

	ss << pattern.tostring(*classDecoder);

	return ss.str();
}

std::string Document::toString(featureItr featItr)
{
	std::stringstream ss;

	Pattern p = featItr->first;
	double v = featItr->second;

	ss << p.tostring(*classDecoder) << " (" << v << ")";

	return ss.str();
}

std::string Document::toString()
{
	std::stringstream ss;

	ss << "(" << documentId << ") - " << fileName;

	return ss.str();
}

bool operator==(const Document& lhs, const Document& rhs)
{
	if (lhs.documentId == rhs.documentId && lhs.fileName == rhs.fileName)
	{
		return true;
	}
	return false;
}

bool operator!=(const Document& lhs, const Document& rhs)
{
	return !(lhs == rhs);
}

/*
 * Document.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: lonrust
 */

#include "Document.h"



#include <boost/foreach.hpp>

typedef std::unordered_map<const Pattern, double> umap;

Document::Document(int documentId, const std::string& fileName,  boost::shared_ptr<ClassDecoder> classDe1coder)
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
 * If pattern exists in the document, double returns the old value of pattern
 * otherwise the behaviour is undefined
 */
double Document::updateValue(const Pattern& pattern, double newValue)
{
	 std::unordered_map<const Pattern,double>::const_iterator iter = data.find (pattern);

	  if ( iter != data.end() )
	  {
		  double oldValue = iter->second;
	     data[pattern] = newValue;

	    return oldValue;
	  }

	return 0;
}

void Document::printPatterns()
{
	std::cout << "(" << documentId << " --- " << fileName << ")" << std::endl;
	BOOST_FOREACH( umap::value_type v, data ) {
		Pattern p = v.first;
	    std::cout << "[" << p.tostring(*classDecoder) << "]" << v.second << std::endl;
	}
}

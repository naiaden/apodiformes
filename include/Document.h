/*
 * Document.h
 *
 *  Created on: Oct 26, 2013
 *      Author: lonrust
 */

#ifndef DOCUMENT_H_
#define DOCUMENT_H_


#include <patternmodel.h>
#include <pattern.h>

#include <classdecoder.h>

#include <boost/shared_ptr.hpp>


class Document : public PatternMap<double>
{
private:
	int documentId;
	std::string fileName;
	boost::shared_ptr<ClassDecoder> classDecoder;

public:
	Document(int documentId, const std::string& fileName, boost::shared_ptr<ClassDecoder> classDecoder);
	virtual ~Document();

	double updateValue(const Pattern& pattern, double newValue);

	void printPatterns();
};

#endif /* DOCUMENT_H_ */


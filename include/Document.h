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

	typedef std::unordered_map<const Pattern, double>::iterator featureItr;
	typedef std::unordered_map<const Pattern, double>::const_iterator const_featureItr;

	featureItr begin() { return data.begin(); }
	const_featureItr begin() const { return data.begin(); }
	featureItr end() { return data.end(); }
	const_featureItr end() const { return data.end(); }





	Document(int documentId, const std::string& fileName, boost::shared_ptr<ClassDecoder> classDecoder);
	virtual ~Document();

	boost::shared_ptr< ClassDecoder> getClassDecoder();

	double getValue(const Pattern& pattern);
	double updateValue(const Pattern& pattern, double newValue);

	void printPatterns();

	std::string toString(const Pattern& pattern);
	std::string toString(featureItr featItr);
	std::string toString();
};

#endif /* DOCUMENT_H_ */


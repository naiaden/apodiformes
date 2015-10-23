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

class Document: public PatternMap<double>
{
private:
	int documentId;
	std::string fileName;
	ClassDecoder* classDecoder;

public:

	int getDocumentId();
	std::string getFileName();

	typedef std::unordered_map<const Pattern, double>::iterator featureItr;
	typedef std::unordered_map<const Pattern, double>::const_iterator const_featureItr;

	featureItr begin()
	{
		return data.begin();
	}
	const_featureItr begin() const
	{
		return data.begin();
	}
	featureItr end()
	{
		return data.end();
	}
	const_featureItr end() const
	{
		return data.end();
	}

	friend bool operator==(const Document& lhs, const Document& rhs);
	friend bool operator!=(const Document& lhs, const Document& rhs);

	Document(int documentId, const std::string& fileName, ClassDecoder* classDecoder);
	virtual ~Document();

	ClassDecoder* getClassDecoder();

	double getValue(const Pattern& pattern) const;
	double updateValue(const Pattern& pattern, double newValue);

	void printPatterns();

	std::string toString(const Pattern& pattern) const;
	std::string toString(featureItr featItr);
	std::string toString();
};


#endif /* DOCUMENT_H_ */


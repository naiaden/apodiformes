/*
 * TFIDF.cpp
 *
 *  Created on: Oct 28, 2013
 *      Author: lonrust
 */

#include "TFIDF.h"

TFIDF::TFIDF()
{
	// TODO Auto-generated constructor stub
}

TFIDF::~TFIDF()
{
	// TODO Auto-generated destructor stub
}

int TFIDF::getFrequency(const Pattern& pattern)
{
	int frequency = 0;

	for (VectorSpaceModel::documentItr docItr = begin(); docItr != end(); ++docItr)
	{
		boost::shared_ptr<ClassDecoder> decoder = docItr->getClassDecoder();
		for (Document::featureItr featItr = docItr->begin(); featItr != docItr->end(); ++featItr)
		{
			if (featItr->first == pattern)
			{
				frequency += featItr->second;
			}
		}
	}

	return frequency;
}

int TFIDF::getFrequency(const Pattern& pattern, const Document& document)
{
	return static_cast<int>(document.getValue(pattern));
}

double TFIDF::getTF(const Pattern& pattern, const Document& document)
{
	return log(getFrequency(pattern, document) + 1);
}

double TFIDF::getIDF(const Pattern& pattern)
{
	const int numberOfDocuments = this->numberOfDocuments();
	int numberOfDocumentsWithPattern = 0;

	for (documentItr docItr = begin(); docItr != end(); ++docItr)
	{
		if (getFrequency(pattern, *docItr) > 0)
		{
			++numberOfDocumentsWithPattern;
		}
	}

	return numberOfDocuments / numberOfDocumentsWithPattern;
}

double TFIDF::getTFIDF(const Pattern& pattern, const Document& document)
{
	double tf = getTF(pattern, document);
	double idf = getIDF(pattern);

	return tf * idf;
}

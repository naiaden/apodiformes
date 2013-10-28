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

TFIDF::TFIDF(const VectorSpaceModel& vsm)
{
	// convert
}

TFIDF::~TFIDF()
{
	// TODO Auto-generated destructor stub
}

int TFIDF::getFrequency(const Pattern& pattern)
{
	int frequency = 0;

	for(VectorSpaceModel::documentItr docItr = begin(); docItr != end(); ++docItr)
	{
		boost::shared_ptr< ClassDecoder> decoder = docItr->getClassDecoder();
		for(Document::featureItr featItr = docItr->begin(); featItr != docItr->end(); ++featItr)
		{
			if(featItr->first == pattern)
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

int TFIDF::getTF(const Pattern& pattern, const Document& document)
{
	return log(getFrequency(pattern, document));
}

double TFIDF::getIDF(const Pattern& pattern)
{
 return 0.0;
}

double TFIDF::getTFIDF(const Pattern& pattern)
{
return 0.0;
}

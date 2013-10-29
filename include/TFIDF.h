/*
 * TFIDF.h
 *
 *  Created on: Oct 28, 2013
 *      Author: lonrust
 */

#ifndef TFIDF_H_
#define TFIDF_H_

#include <pattern.h>
#include "Document.h"
#include "VectorSpaceModel.h"

/**
 * tf-idf is computed using a log-scaled frequency over the term frequency and
 * a log-scaled inverse document frequency.
 */
class TFIDF: public VectorSpaceModel
{
public:
	TFIDF(const IndexedPatternModel<>& patternModel);

	double getTFIDF(const Pattern& pattern, const Document& document);

private:
	int getFrequency(const Pattern& pattern);
	int getFrequency(const Pattern& pattern, const Document& document);

	virtual ~TFIDF();

	double getTF(const Pattern& pattern, const Document& document);
	double getIDF(const Pattern& pattern);

};

#endif /* TFIDF_H_ */

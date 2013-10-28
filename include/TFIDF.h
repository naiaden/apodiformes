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

class TFIDF : public VectorSpaceModel
{
	private:
	int getFrequency(const Pattern& pattern);
	int getFrequency(const Pattern& pattern, const Document& document);

public:
	TFIDF();
	TFIDF(const VectorSpaceModel& vsm);
	virtual ~TFIDF();

	int getTF(const Pattern& pattern, const Document& document);
	double getIDF(const Pattern& pattern);
	double getTFIDF(const Pattern& pattern);
};

#endif /* TFIDF_H_ */

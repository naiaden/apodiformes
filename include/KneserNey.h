/*
 * KneserNey.h
 *
 *  Created on: Oct 28, 2013
 *      Author: lonrust
 */

#ifndef KNESERNEY_H_
#define KNESERNEY_H_

#include "VectorSpaceModel.h"
#include <pattern.h>

class KneserNey: public VectorSpaceModel
{
private:
	int n1, n2, n3, n4;

public:
	KneserNey(const IndexedPatternModel<>& patternModel);
	virtual ~KneserNey();

	void computeFrequencyStats();

	double wordChanceForOrder(const Pattern& pattern, int order);
	int patternCount(const Pattern& pattern);
	double discount(int count);

	double y();
};

#endif /* KNESERNEY_H_ */

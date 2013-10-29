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



	double rawProbability(const Pattern& pattern, int indentation = 0);
	double smoothedProbability(const Pattern& pattern, int indentation = 0);

	double interpolationFactor(const Pattern& pattern, int indentation = 0);
	double N(const Pattern& pattern, int& N1, int& N2, int& N3);

	double wordChanceForOrder(const Pattern& pattern, int order);
	int patternCount(const Pattern& pattern);
	double discount(int count);

	double y();

public:

	KneserNey(const IndexedPatternModel<>& patternModel, boost::shared_ptr<ClassDecoder> classDecoder);
	virtual ~KneserNey();
	double getSmoothedValue(const Pattern& pattern, int indentation =0);

	void computeFrequencyStats();

};

#endif /* KNESERNEY_H_ */

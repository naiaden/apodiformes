/*
 * VectorSpaceModel.h
 *
 *  Created on: Oct 24, 2013
 *      Author: lonrust
 */

#ifndef VECTORSPACEMODEL_H_
#define VECTORSPACEMODEL_H_

#include <pattern.h>

class VectorSpaceModel : public PatternMap<double> {
public:
	VectorSpaceModel();
	virtual ~VectorSpaceModel();

	double updateValue(const Pattern& pattern, double newValue);
};

#endif /* VECTORSPACEMODEL_H_ */

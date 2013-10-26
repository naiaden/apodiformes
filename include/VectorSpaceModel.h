/*
 * VectorSpaceModel.h
 *
 *  Created on: Oct 24, 2013
 *      Author: lonrust
 */

#ifndef VECTORSPACEMODEL_H_
#define VECTORSPACEMODEL_H_

#include <pattern.h>
#include "Document.h"
#include <vector>

class VectorSpaceModel {
private:
	std::vector< Document> documents;

public:
	VectorSpaceModel();
	virtual ~VectorSpaceModel();

	void addDocument(const Document& newDocument);

	void printVectorSpace();
};



#endif /* VECTORSPACEMODEL_H_ */

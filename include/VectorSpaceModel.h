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


	typedef std::vector< Document>::iterator documentItr;
	typedef std::vector< Document>::const_iterator const_documentItr;

	documentItr begin() { return documents.begin(); }
	const_documentItr begin() const { return documents.begin(); }
	documentItr end() { return documents.end(); }
	const_documentItr end() const { return documents.end(); }


	VectorSpaceModel();
	virtual ~VectorSpaceModel();

	void addDocument(const Document& newDocument);

	void printVectorSpace();

	int numberOfDocuments();
};



#endif /* VECTORSPACEMODEL_H_ */

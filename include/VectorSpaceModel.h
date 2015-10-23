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
#include <patternmodel.h>
#include <vector>

#include <classdecoder.h>

#include <boost/shared_ptr.hpp>

class VectorSpaceModel {
private:
	std::vector< Document> documents;


protected:
	ClassDecoder* classDecoder;
	IndexedPatternModel<>* patternModel;

public:


	void test();
	IndexedPatternModel<>* getPatternModel();

	virtual double computeSimularity(const Document& document) = 0;

	typedef std::vector< Document>::iterator documentItr;
	typedef std::vector< Document>::const_iterator const_documentItr;

	documentItr begin() { return documents.begin(); }
	const_documentItr begin() const { return documents.begin(); }
	documentItr end() { return documents.end(); }
	const_documentItr end() const { return documents.end(); }


	bool documentExists(const Document& document);

	VectorSpaceModel(IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder);
	virtual ~VectorSpaceModel();

	void addDocument(const Document& newDocument);

	void printVectorSpace();

	int numberOfDocuments();
};



#endif /* VECTORSPACEMODEL_H_ */

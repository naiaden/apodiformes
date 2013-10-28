/*
 * VectorSpaceModel.cpp
 *
 *  Created on: Oct 24, 2013
 *      Author: lonrust
 */

#include "VectorSpaceModel.h"

#include "Document.h"
#include <boost/foreach.hpp>

VectorSpaceModel::VectorSpaceModel() {
	// TODO Auto-generated constructor stub

}

VectorSpaceModel::~VectorSpaceModel() {
	// TODO Auto-generated destructor stub
}

void VectorSpaceModel::addDocument(const Document& newDocument)
{
	documents.push_back(newDocument);
}

void VectorSpaceModel::printVectorSpace()
{
	BOOST_FOREACH( Document d, documents)
	{
		d.printPatterns();
		std::cout << std::endl;
	}
}

int VectorSpaceModel::numberOfDocuments()
{
	return documents.size();
}

/*
 * VectorSpaceModel.cpp
 *
 *  Created on: Oct 24, 2013
 *      Author: lonrust
 */

#include "VectorSpaceModel.h"

#include "Document.h"
#include <boost/foreach.hpp>

VectorSpaceModel::VectorSpaceModel(IndexedPatternModel<>* patternModel, ClassDecoder* classDecoder)
		: patternModel(patternModel), classDecoder(classDecoder)
{
	// TODO Auto-generated constructor stub

}

VectorSpaceModel::~VectorSpaceModel()
{
	// TODO Auto-generated destructor stub
}

IndexedPatternModel<>* VectorSpaceModel::getPatternModel()
{
	return patternModel;
}

bool VectorSpaceModel::documentExists(const Document& document)
{
	BOOST_FOREACH( Document d, documents){
	if(d == document)
	{
		return true;
	}
}

return false;
}

void VectorSpaceModel::addDocument(const Document& newDocument)
{
	documents.push_back(newDocument);
}

void VectorSpaceModel::printVectorSpace()
{
	BOOST_FOREACH( Document d, documents){
	d.printPatterns();
	std::cout << std::endl;
}
}

int VectorSpaceModel::numberOfDocuments()
{
	return documents.size();
}

void VectorSpaceModel::test()
{

	PatternModelOptions options;
		options.DOREVERSEINDEX = true;
		options.DOSKIPGRAMS = true;
		options.MINTOKENS = 1;
		options.MAXLENGTH = 5;

	BOOST_FOREACH( Document document, documents )
		{
		std::string fileName = document.getFileName();


			int k = 0;

			std::cout << "Iterating over all patterns in " << fileName << std::endl;
			for (IndexedPatternModel<>::iterator iter = patternModel->begin(); iter != patternModel->end(); iter++)
			{
				const Pattern pattern = iter->first;
				const IndexedData data = iter->second;

				double value = patternModel->occurrencecount(pattern);

				document.updateValue(pattern, value);

	//			std::cout << "-" << document.toString(pattern) << "," << document.getValue(pattern) << std::endl;

				++k;
			}

				std::cout << ">>> " << k << std::endl;

		}
}

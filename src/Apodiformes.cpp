/*
 * Apodiformes.cpp
 *
 *  Created on: Oct 24, 2013
 *      Author: lonrust
 */

#include <iostream>

#include <string>
#include <iostream>
#include <fstream>

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#include <boost/foreach.hpp>

#include "VectorSpaceModel.h"

int main(int argc, char** argv)
{
	std::cout << "STRAK" << std::endl;

	std::cerr << "Class encoding corpus..." << std::endl;
	system("colibri-classencode docs/aiw.tok");

	PatternModelOptions options;
	options.DOREVERSEINDEX = true;
	options.DOSKIPGRAMS = true;

	const std::string collectionClassFile = "docs/aiw.tok.colibri.cls";

	ClassDecoder collectionClassDecoder = ClassDecoder(collectionClassFile);
	ClassEncoder collectionClassEncoder = ClassEncoder(collectionClassFile);

	std::string collectionInputFileName = "docs/aiw.tok.colibri.dat";
	std::string collectionOutputFileName = "docs/aiw.tok.colibri.patternmodel";

	IndexedPatternModel<> collectionIndexedModel;
	collectionIndexedModel.train(collectionInputFileName, options);

	VectorSpaceModel vsm = VectorSpaceModel();

	std::cerr << "Iterating over all patterns in all docs" << std::endl;
		for (IndexedPatternModel<>::iterator iter = collectionIndexedModel.begin(); iter != collectionIndexedModel.end(); iter++)
		{
			const Pattern pattern = iter->first;
			const IndexedData data = iter->second;

			double value = collectionIndexedModel.occurrencecount(pattern);
			vsm.insert(pattern, value);

			std::cout << ">" << pattern.tostring(collectionClassDecoder) << "," << vsm[pattern] << std::endl;

		}

	std::vector< std::string> documentInputFiles = std::vector< std::string>();
	documentInputFiles.push_back(std::string("docs/aiw-1.tok"));
	documentInputFiles.push_back(std::string("docs/aiw-2.tok"));
	documentInputFiles.push_back(std::string("docs/aiw-3.tok"));

	BOOST_FOREACH( std::string fileName, documentInputFiles )
	{
		std::cout << fileName << std::endl;
	}

	std::cout << "ALS EEN REIGER" << std::endl;
}

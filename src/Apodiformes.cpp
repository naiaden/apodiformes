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

#include "VectorSpaceModel.h"

int main(int argc, char** argv)
{
	std::cout << "STRAK" << std::endl;



	std::cerr << "Class encoding corpus..." << std::endl;
	system("colibri-classencode /tmp/hamlet.txt");

//	std::cerr << "Class decoding corpus..." << std::endl;
//	system("colibri-classdecode -c /tmp/hamlet.colibri.cls -f /tmp/hamlet.colibri.dat");

	PatternModelOptions options;
	options.DOREVERSEINDEX = true;
	options.DOFIXEDSKIPGRAMS = true;

	const std::string classfile = "/tmp/hamlet.colibri.cls";
	ClassDecoder classdecoder = ClassDecoder(classfile);
	ClassEncoder classencoder = ClassEncoder(classfile);

	std::string infilename = "/tmp/hamlet.colibri.dat";
	std::string outputfilename = "/tmp/data.colibri.patternmodel";

	std::cerr << std::endl;
	std::cerr << "Building indexed model" << std::endl;
	IndexedPatternModel<> indexedmodel;
	indexedmodel.train(infilename, options);

	VectorSpaceModel vsm = VectorSpaceModel();

	std::cerr << "Iterating over all patterns" << std::endl;
	for (IndexedPatternModel<>::iterator iter = indexedmodel.begin(); iter != indexedmodel.end(); iter++)
	{
		const Pattern pattern = iter->first;
		const IndexedData data = iter->second;

		double value = indexedmodel.occurrencecount(pattern);
		vsm.insert(pattern, value);

		std::cout << ">" << pattern.tostring(classdecoder) << "," << vsm[pattern] << std::endl;

	}

	std::cout << "ALS EEN REIGER" << std::endl;
}

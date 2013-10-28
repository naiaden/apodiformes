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
#include <boost/shared_ptr.hpp>

#include "VectorSpaceModel.h"
#include "KneserNey.h"
#include "TFIDF.h"

typedef boost::shared_ptr<ClassDecoder> ClassDecoder_ptr;

int main(int argc, char** argv)
{
	std::cout << "STRAK" << std::endl;

	std::cerr << "Class encoding corpus..." << std::endl;
	system("colibri-classencode docs/aiw.tok");

	PatternModelOptions options;
	options.DOREVERSEINDEX = true;
	options.DOSKIPGRAMS = true;
	options.MINTOKENS = 1;
	options.MAXLENGTH = 5;

	const std::string collectionClassFile = "docs/aiw.tok.colibri.cls";

	ClassEncoder collectionClassEncoder = ClassEncoder(collectionClassFile);

	ClassDecoder_ptr collectionClassDecoderPtr(new ClassDecoder(collectionClassFile));

	std::string collectionInputFileName = "docs/aiw.tok.colibri.dat";
	std::string collectionOutputFileName = "docs/aiw.tok.colibri.patternmodel";

	IndexedPatternModel<> collectionIndexedModel;
	collectionIndexedModel.train(collectionInputFileName, options);



	std::cout << "Iterating over all patterns in all docs" << std::endl;
	for (IndexedPatternModel<>::iterator iter = collectionIndexedModel.begin(); iter != collectionIndexedModel.end(); iter++)
	{
		const Pattern pattern = iter->first;
		const IndexedData data = iter->second;

		double value = collectionIndexedModel.occurrencecount(pattern);
		std::cout << ">" << pattern.tostring(*collectionClassDecoderPtr) << "," << value << std::endl;

	}

	KneserNey vsm = KneserNey(collectionIndexedModel);

	std::vector< std::string> documentInputFiles = std::vector< std::string>();
	documentInputFiles.push_back(std::string("docs/aiw-1.tok"));
	documentInputFiles.push_back(std::string("docs/aiw-2.tok"));
	documentInputFiles.push_back(std::string("docs/aiw-3.tok"));

	int docCntr = 0;
	BOOST_FOREACH( std::string fileName, documentInputFiles )
	{
		Document document = Document(docCntr++, fileName, collectionClassDecoderPtr);

		const std::string command = std::string("colibri-classencode -c docs/aiw.tok.colibri.cls ") + fileName;
		system( command.c_str() );

		const std::string documentClassFile = fileName + ".cls";
		const std::string inputFileName = fileName + ".colibri.dat";
		const std::string outputFileName = fileName + ".colibri.patternmodel";

		ClassDecoder documentClassDecoder = ClassDecoder(documentClassFile);

		IndexedPatternModel<> documentModel;
		documentModel.train(inputFileName, options);

		int k = 0;

		std::cout << "Iterating over all patterns in " << fileName << std::endl;
		for (IndexedPatternModel<>::iterator iter = documentModel.begin(); iter != documentModel.end(); iter++)
		{
			const Pattern pattern = iter->first;
			const IndexedData data = iter->second;

			double value = documentModel.occurrencecount(pattern);

			document.updateValue(pattern, value);

//			std::cout << "-" << document.toString(pattern) << "," << document.getValue(pattern) << std::endl;

			++k;
		}

			std::cout << ">>> " << k << std::endl;

		vsm.addDocument(document);

	}

	std::cout << "===========================================" << std::endl;

	IndexedPatternModel<> ipm = vsm.getPatternModel();

	int docCntr1 = 0;
		BOOST_FOREACH( std::string fileName, documentInputFiles )
		{
			Document document = Document(docCntr1++, fileName, collectionClassDecoderPtr);

			const std::string command = std::string("colibri-classencode -c docs/aiw.tok.colibri.cls ") + fileName;
			system( command.c_str() );

			const std::string documentClassFile = fileName + ".cls";
			const std::string inputFileName = fileName + ".colibri.dat";
			const std::string outputFileName = fileName + ".colibri.patternmodel";

			ClassDecoder documentClassDecoder = ClassDecoder(documentClassFile);



			int k = 0;

			std::cout << "Iterating over all patterns in " << fileName << std::endl;
			for (IndexedPatternModel<>::iterator iter = ipm.begin(); iter != ipm.end(); iter++)
			{
				const Pattern pattern = iter->first;
				const IndexedData data = iter->second;

				double value = ipm.occurrencecount(pattern);

				document.updateValue(pattern, value);

	//			std::cout << "-" << document.toString(pattern) << "," << document.getValue(pattern) << std::endl;

				++k;
			}

				std::cout << ">>> " << k << std::endl;

		}









	std::cout << "The vector space contains " << vsm.numberOfDocuments() << " documents" << std::endl;
	for(VectorSpaceModel::documentItr docItr = vsm.begin(); docItr != vsm.end(); ++docItr)
	{
		std::cout << docItr->toString() << std::endl;
		boost::shared_ptr< ClassDecoder> decoder = docItr->getClassDecoder();
		for(Document::featureItr featItr = docItr->begin(); featItr != docItr->end(); ++featItr)
		{
//			std::cout << docItr->toString(featItr) << "[" << vsm.getTFIDF(featItr->first, *docItr) << "]" << std::endl;
		}
		std::cout << std::endl;
	}


//	vsm.computeFrequencyStats();
	vsm.test();


	std::cout << "ALS EEN REIGER" << std::endl;
}

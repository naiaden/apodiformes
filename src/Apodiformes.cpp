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
#include <boost/algorithm/string/join.hpp>

#include "VectorSpaceModel.h"
#include "KneserNey.h"
#include "TFIDF.h"

int main(int argc, char** argv)
{
	std::cout << "STRAK" << std::endl;

	const std::string inputDirectory = "docs/alice/";
	const std::string generatedDirectory = "docs/generated/";

	std::vector<std::string> trainInputFiles = std::vector<std::string>();
		trainInputFiles.push_back(std::string("aiw1-1.tok"));
		trainInputFiles.push_back(std::string("aiw1-2.tok"));
		trainInputFiles.push_back(std::string("aiw1-3.tok"));
		trainInputFiles.push_back(std::string("aiw1-4.tok"));
		trainInputFiles.push_back(std::string("aiw2-1.tok"));
		trainInputFiles.push_back(std::string("aiw2-2.tok"));
		trainInputFiles.push_back(std::string("aiw2-3.tok"));
		trainInputFiles.push_back(std::string("aiw2-4.tok"));

		std::vector<std::string> testInputFiles = std::vector<std::string>();
		testInputFiles.push_back(std::string("aiw4-1.tok"));
		testInputFiles.push_back(std::string("aiw4-2.tok"));
		testInputFiles.push_back(std::string("aiw4-3.tok"));
		testInputFiles.push_back(std::string("aiw4-4.tok"));
		testInputFiles.push_back(std::string("aiw3-1.tok"));
		testInputFiles.push_back(std::string("aiw3-2.tok"));

		std::vector<std::string> allFiles = trainInputFiles;
		allFiles.insert(allFiles.end(), testInputFiles.begin(), testInputFiles.end());
		std::string allFileNames = std::string(inputDirectory).append(boost::algorithm::join(allFiles, std::string(" ").append(inputDirectory)));


	PatternModelOptions options;
	options.DOREVERSEINDEX = true;
	options.DOSKIPGRAMS = false;
	options.MINTOKENS = 1;
	options.MAXLENGTH = 5;
	options.QUIET = true;


	int indentation = 0;

	std::cout << std::string(indentation++, '\t') << "+ Creating collection files" << std::endl;

	std::cout << std::string(indentation, '\t') << "Class encoding collection files..." << std::endl;

	system(std::string("colibri-classencode -o ").append(generatedDirectory).append("collection -u ").append(allFileNames).c_str());

	const std::string collectionClassFile = generatedDirectory + "collection.colibri.cls";
	ClassEncoder collectionClassEncoder = ClassEncoder(collectionClassFile);

	boost::shared_ptr<ClassDecoder> collectionClassDecoderPtr(new ClassDecoder(collectionClassFile));

	std::string collectionInputFileName = generatedDirectory + "collection.colibri.dat";
	std::string collectionOutputFileName = generatedDirectory + "collection.colibri.patternmodel";

	IndexedPatternModel<> collectionIndexedModel;
	std::cout << std::string(indentation, '\t') << "Indexing collection" << std::endl;
	collectionIndexedModel.train(collectionInputFileName, options);

	std::cout << std::string(--indentation, '\t') << "- Creating collection files" << std::endl;


	KneserNey trainLanguageModel = KneserNey(collectionIndexedModel, collectionClassDecoderPtr);



	std::cout << std::string(indentation++, '\t') << "+ Processing training files" << std::endl;

	int docCntr = 0;
	BOOST_FOREACH( std::string fileName, trainInputFiles )
	{
		std::cout << std::string(indentation++, '\t') << "+ " << fileName << std::endl;
		std::cout << std::string(indentation, '\t') << "Encoding document" << std::endl;
		Document document = Document(docCntr++, fileName, collectionClassDecoderPtr);

		const std::string command = std::string("colibri-classencode -o ") + generatedDirectory + fileName + "-c " + generatedDirectory + "collection.colibri.cls " + inputDirectory + fileName;
		system( command.c_str() );

//		const std::string documentClassFile = fileName + ".cls";
//		const std::string inputFileName = fileName + ".colibri.dat";
//		const std::string outputFileName = fileName + ".colibri.patternmodel";
//
//		IndexedPatternModel<> documentModel;
//		documentModel.train(inputFileName, options);
//
//		std::cout << std::string(indentation, '\t') << "Iterating over all patterns" << std::endl;
//		for (IndexedPatternModel<>::iterator iter = documentModel.begin(); iter != documentModel.end(); iter++)
//		{
//			const Pattern pattern = iter->first;
//			const IndexedData data = iter->second;
//
//			double value = documentModel.occurrencecount(pattern);
//
//			document.updateValue(pattern, value);
//		}
//
//		trainLanguageModel.addDocument(document);

		std::cout << std::string(--indentation, '\t') << "- " << fileName << std::endl;
	}
//
//	std::cout << std::string(--indentation, '\t') << "- Processing training files" << std::endl;
//
//
//	std::cout << std::string(indentation++, '\t') << "+ Computing frequency stats for Kneser-Ney" << std::endl;
//	trainLanguageModel.computeFrequencyStats();
//	std::cout << std::string(--indentation, '\t') << "- Computing frequency stats for Kneser-Ney" << std::endl;
//
//
//
//
//
//
//	std::cout << std::string(indentation++, '\t') << "+ Processing test files" << std::endl;
//
//	int docCntr = 0;
//	BOOST_FOREACH( std::string fileName, testInputFiles )
//	{
//		std::cout << std::string(indentation++, '\t') << "+ " << fileName << std::endl;
//		std::cout << std::string(indentation, '\t') << "Encoding document" << std::endl;
//		Document document = Document(docCntr++, fileName, collectionClassDecoderPtr);
//
//		const std::string command = std::string("colibri-classencode -c docs/aiw.tok.colibri.cls ") + fileName;
//		system( command.c_str() );
//
//		const std::string documentClassFile = fileName + ".cls";
//		const std::string inputFileName = fileName + ".colibri.dat";
//		const std::string outputFileName = fileName + ".colibri.patternmodel";
//
//		IndexedPatternModel<> documentModel;
//		documentModel.train(inputFileName, options);
//
//		std::cout << std::string(indentation, '\t') << "Iterating over all patterns" << std::endl;
//		for (IndexedPatternModel<>::iterator iter = documentModel.begin(); iter != documentModel.end(); iter++)
//		{
//			const Pattern pattern = iter->first;
//			const IndexedData data = iter->second;
//
//			double value = documentModel.occurrencecount(pattern);
//
//			document.updateValue(pattern, value);
//		}
//
//		trainLanguageModel.addDocument(document);
//
//		std::cout << std::string(--indentation, '\t') << "- " << fileName << std::endl;
//	}
//
//	std::cout << std::string(--indentation, '\t') << "- Processing test files" << std::endl;



//	std::cout << "The vector space contains " << vsm.numberOfDocuments() << " documents" << std::endl;
//	for(VectorSpaceModel::documentItr docItr = vsm.begin(); docItr != vsm.end(); ++docItr)
//	{
//		std::cout << docItr->toString() << std::endl;
//		boost::shared_ptr< ClassDecoder> decoder = docItr->getClassDecoder();
//		for(Document::featureItr featItr = docItr->begin(); featItr != docItr->end(); ++featItr)
//		{
////			std::cout << docItr->toString(featItr) << "[" << vsm.getTFIDF(featItr->first, *docItr) << "]" << std::endl;
//		}
//		std::cout << std::endl;
//	}
//
//
//	vsm.computeFrequencyStats();
////	vsm.test();
//

	std::cout << "ALS EEN REIGER" << std::endl;
}


















//	std::cout << "Iterating over all patterns in all docs" << std::endl;
//	for (IndexedPatternModel<>::iterator iter = collectionIndexedModel.begin(); iter != collectionIndexedModel.end(); iter++)
//	{
//		const Pattern pattern = iter->first;
//		const IndexedData data = iter->second;
//
//		double value = collectionIndexedModel.occurrencecount(pattern);
//		std::cout << ">" << pattern.tostring(*collectionClassDecoderPtr) << "," << value << std::endl;
//
//	}
//
//	KneserNey vsm = KneserNey(collectionIndexedModel, collectionClassDecoderPtr);
//



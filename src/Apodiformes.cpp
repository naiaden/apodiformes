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
#include "File.h"

int main(int argc, char** argv)
{
	std::cout << "STRAK" << std::endl;

	const std::string inputDirectory = "docs/alice";
	const std::string generatedDirectory = "docs/generated";
	const std::string collectionName = "alice";

	std::vector<TrainFile> trainInputFiles = std::vector<TrainFile>();
	trainInputFiles.push_back(TrainFile("aiw1-1", "tok", inputDirectory));
	trainInputFiles.push_back(TrainFile("aiw1-2", "tok", inputDirectory));
	trainInputFiles.push_back(TrainFile("aiw1-3", "tok", inputDirectory));
	trainInputFiles.push_back(TrainFile("aiw1-4", "tok", inputDirectory));
	trainInputFiles.push_back(TrainFile("aiw2-1", "tok", inputDirectory));
	trainInputFiles.push_back(TrainFile("aiw2-2", "tok", inputDirectory));
	trainInputFiles.push_back(TrainFile("aiw2-3", "tok", inputDirectory));
	trainInputFiles.push_back(TrainFile("aiw2-4", "tok", inputDirectory));

	std::vector<TestFile> testInputFiles = std::vector<TestFile>();
	testInputFiles.push_back(TestFile("aiw4-1", "tok", inputDirectory));
	testInputFiles.push_back(TestFile("aiw4-2", "tok", inputDirectory));
	testInputFiles.push_back(TestFile("aiw4-3", "tok", inputDirectory));
	testInputFiles.push_back(TestFile("aiw4-4", "tok", inputDirectory));
	testInputFiles.push_back(TestFile("aiw3-1", "tok", inputDirectory));
	testInputFiles.push_back(TestFile("aiw3-2", "tok", inputDirectory));

	std::string allFileNames;
	BOOST_FOREACH( TrainFile f, trainInputFiles) // generate a list of all file names
	{
		allFileNames += f.getPath() + " ";
	}

	PatternModelOptions options;
	options.DOREVERSEINDEX = true;
	options.DOSKIPGRAMS = false;
	options.MINTOKENS = 1;
	options.MAXLENGTH = 5;
	options.QUIET = true;


	int indentation = 0;

	std::cout << std::string(indentation++, '\t') << "+ Creating collection files" << std::endl;
	std::cout << std::string(indentation, '\t') << "Class encoding collection files..." << std::endl;

	ColibriFile collectionCorpusFile = ColibriFile(collectionName, "colibri.cls", generatedDirectory, ColibriFile::Type::CORPUS);
	ColibriFile collectionEncodedFile = ColibriFile(collectionName, "colibri.dat", generatedDirectory, ColibriFile::Type::ENCODED); //collectionInputFileName
	ColibriFile collectionPatternFile = ColibriFile(collectionName, "colibri.dat", generatedDirectory, ColibriFile::Type::PATTERNMODEL); //collectionOutputFileName

	std::string collectionClassEncodeCommand = std::string("colibri-classencode -o ").append(collectionCorpusFile.getPath(false)).append(" -u ").append(allFileNames);
	std::cout << "Executing command: " << collectionClassEncodeCommand << std::endl;
	system(collectionClassEncodeCommand.c_str());

	ClassEncoder collectionClassEncoder = ClassEncoder(collectionCorpusFile.getPath());
	boost::shared_ptr<ClassDecoder> collectionClassDecoderPtr(new ClassDecoder(collectionCorpusFile.getPath()));


	IndexedPatternModel<> collectionIndexedModel;
	std::cout << std::string(indentation, '\t') << "Indexing collection" << std::endl;
	collectionIndexedModel.train(collectionEncodedFile.getPath(), options);

	std::cout << std::string(--indentation, '\t') << "- Creating collection files" << std::endl;


	KneserNey trainLanguageModel = KneserNey(collectionIndexedModel, collectionClassDecoderPtr);



	std::cout << std::string(indentation++, '\t') << "+ Processing training files" << std::endl;

	int docCntr = 0;
	BOOST_FOREACH( TrainFile tf, trainInputFiles )
	{
		std::cout << std::string(indentation++, '\t') << "+ " << tf.getPath() << std::endl;
		std::cout << std::string(indentation, '\t') << "Encoding document" << std::endl;
		Document document = Document(docCntr++, tf.getPath(), collectionClassDecoderPtr);
//
//		const std::string command = std::string("colibri-classencode -o ") + generatedDirectory + fileName + " -c " + generatedDirectory + "collection.colibri.cls " + inputDirectory + fileName;
//		std::cout << "Executing command: " << command << std::endl;
//		system( command.c_str() );
//
//		const std::string documentClassFile = generatedDirectory + fileName + ".cls";
//		const std::string inputFileName = generatedDirectory + fileName + ".colibri.dat";
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
////
////		trainLanguageModel.addDocument(document);

		std::cout << std::string(--indentation, '\t') << "- " << tf.getPath() << std::endl;
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



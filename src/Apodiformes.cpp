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

#include "Common.h"

int main(int argc, char** argv)
{
	std::cout << "STRAK" << std::endl;

	const std::string inputDirectory = "docs/alice/";
	const std::string generatedDirectory = "docs/generated/";
	const std::string collectionName = "alice";
	const std::string colibriEncoder = "~/Software/colibri-core/src/colibri-classencode";

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
{	allFileNames += f.getPath() + " ";
}

	PatternModelOptions options;
	options.DOREVERSEINDEX = true;
	options.DOSKIPGRAMS = false;
	options.MINTOKENS = 1;
	options.MAXLENGTH = 5;
	options.QUIET = true;

	int indentation = 0;

	std::cout << indent(indentation++) << "+ Creating collection files" << std::endl;
	std::cout << indent(indentation) << "Class encoding collection files..." << std::endl;

	ColibriFile collectionCorpusFile = ColibriFile(collectionName, "colibri.cls", generatedDirectory,
	        ColibriFile::Type::CORPUS);
	ColibriFile collectionEncodedFile = ColibriFile(collectionName, "colibri.dat", generatedDirectory,
	        ColibriFile::Type::ENCODED); //collectionInputFileName
	ColibriFile collectionPatternFile = ColibriFile(collectionName, "colibri.pattern", generatedDirectory,
	        ColibriFile::Type::PATTERNMODEL); //collectionOutputFileName

	std::string clearGeneratedFiles = std::string("/bin/rm ") + generatedDirectory + "*";
	std::cout << indent(indentation) << "Executing command: " << clearGeneratedFiles << std::endl;
	system(clearGeneratedFiles.c_str());

	std::string collectionClassEncodeCommand = colibriEncoder + " -d " + generatedDirectory + " -o "
	        + collectionCorpusFile.getFileName(false) + " -u " + allFileNames;
	std::cout << indent(indentation) << "Executing command: " << collectionClassEncodeCommand << std::endl;
	system(collectionClassEncodeCommand.c_str());

	ClassEncoder collectionClassEncoder = ClassEncoder(collectionCorpusFile.getPath());
	boost::shared_ptr<ClassDecoder> collectionClassDecoderPtr(new ClassDecoder(collectionCorpusFile.getPath()));

	IndexedPatternModel<> collectionIndexedModel;
	std::cout << indent(indentation) << "Indexing collection" << std::endl;
	collectionIndexedModel.train(collectionEncodedFile.getPath(), options);

	std::cout << indent(--indentation) << "- Creating collection files" << std::endl;

	KneserNey trainLanguageModel = KneserNey(collectionIndexedModel, collectionClassDecoderPtr);

	// ##################################################    Training
	std::cout << indent(indentation++) << "+ Processing training files" << std::endl;

	int docCntr = 0;
	BOOST_FOREACH( TrainFile tf, trainInputFiles )
	{
		std::cout << indent(indentation++) << "+ " << tf.getPath() << std::endl;
		std::cout << indent(indentation) << "+ Encoding document" << std::endl;
		Document document = Document(docCntr++, tf.getPath(), collectionClassDecoderPtr);

		ColibriFile documentCorpusFile = ColibriFile(tf.getFileName(false), "colibri.cls", generatedDirectory, ColibriFile::Type::CORPUS); //documentClassFile
		ColibriFile documentEncodedFile = ColibriFile(tf.getFileName(true), "colibri.dat", generatedDirectory, ColibriFile::Type::ENCODED);//inputFileName
		ColibriFile documentPatternFile = ColibriFile(tf.getFileName(false), "colibri.pattern", generatedDirectory, ColibriFile::Type::PATTERNMODEL);

		const std::string command = colibriEncoder + " -d " + generatedDirectory + " -o " + documentCorpusFile.getFileName(false) + " -c " + collectionCorpusFile.getPath() + " " + tf.getPath();
		std::cout << indent(indentation) << "Executing command: " << command << std::endl;
		system( command.c_str() );
		std::cout << indent(indentation) << "- Encoding done" << std::endl;

		std::cout << indent(indentation) << "+ Training on file: " << documentEncodedFile.getPath() << std::endl;
		IndexedPatternModel<> documentModel;
		documentModel.train(documentEncodedFile.getPath(), options);
		std::cout << indent(indentation) << "- Training on file" << std::endl;

		std::cout << indent(indentation) << "Iterating over all patterns" << std::endl;
		for (IndexedPatternModel<>::iterator iter = documentModel.begin(); iter != documentModel.end(); iter++)
		{
			const Pattern pattern = iter->first;
			const IndexedData data = iter->second;

			double value = documentModel.occurrencecount(pattern);

			document.updateValue(pattern, value);
		}

		trainLanguageModel.addDocument(document);

		std::cout << indent(--indentation) << "- " << tf.getPath() << std::endl;
	}
	std::cout << indent(--indentation) << "- Processing training files" << std::endl;

	std::cout << indent(indentation) << "+ Computing frequency stats for KN" << std::endl;
	trainLanguageModel.computeFrequencyStats();
	std::cout << indent(indentation) << "- Computing frequency stats for KN" << std::endl;

	// ##################################################    Testing
	std::cout << indent(indentation++) << "+ Processing testing files" << std::endl;

	double perplexity = 0;


	docCntr = 0;
	BOOST_FOREACH( TestFile tf, testInputFiles )
	{
		std::cout << indent(indentation++) << "+ " << tf.getPath() << std::endl;
		std::cout << indent(indentation) << "+ Encoding document" << std::endl;
		Document document = Document(docCntr++, tf.getPath(), collectionClassDecoderPtr);

		ColibriFile documentCorpusFile = ColibriFile(tf.getFileName(false), "colibri.cls", generatedDirectory, ColibriFile::Type::CORPUS); //documentClassFile
		ColibriFile documentEncodedFile = ColibriFile(tf.getFileName(true), "colibri.dat", generatedDirectory, ColibriFile::Type::ENCODED);//inputFileName
		ColibriFile documentPatternFile = ColibriFile(tf.getFileName(false), "colibri.pattern", generatedDirectory, ColibriFile::Type::PATTERNMODEL);

		const std::string command = colibriEncoder + " -U" + " -d " + generatedDirectory + " -o " + documentCorpusFile.getFileName(false) + " -c " + collectionCorpusFile.getPath() + " " + tf.getPath();
		std::cout << indent(indentation) << "Executing command: " << command << std::endl;
		system( command.c_str() );
		std::cout << indent(indentation) << "- Encoding done" << std::endl;

		std::cout << indent(indentation) << "+ Testing on file: " << documentEncodedFile.getPath() << std::endl;
		IndexedPatternModel<> documentModel;
		documentModel.train(documentEncodedFile.getPath(), options);
		std::cout << indent(indentation) << "- Testing on file" << std::endl;

		std::cout << indent(indentation) << "Iterating over all patterns" << std::endl;
		for (IndexedPatternModel<>::iterator iter = documentModel.begin(); iter != documentModel.end(); iter++)
		{
			const Pattern pattern = iter->first;

			double value = documentModel.occurrencecount(pattern);


			perplexity += trainLanguageModel.getSmoothedValue(pattern, indentation+1);
		}

		trainLanguageModel.addDocument(document);

		std::cout << indent(--indentation) << "- " << tf.getPath() << std::endl;
	}
	std::cout << indent(--indentation) << "- Processing testing files" << std::endl;






	std::cout << "ALS EEN REIGER" << std::endl;
}


